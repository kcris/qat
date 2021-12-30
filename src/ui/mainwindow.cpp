#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileSystemModel>
#include "TreeModel.h"
#include "OptionsDialog.h"
#include "Catalog.h"

static const QString LOCATEDB_FILEEXT = QStringLiteral(".db");

void buildTreePath(TreeModelItem* pParent, const QStringList & paths)
{
  TreeModelItem* current = pParent;

  foreach (const QString & path, paths)
  {
    TreeModelItem * node = current;

    foreach (const QString & data, path.split("/"))
    {
      current = current->addChild(data);
    }

    current = node;
  }

  //pParent->accept(new PrintIndentedVisitor(0));
}

//
// proxy model for our QFileSystemModel (hides some columns inside)
//
struct BrowseProxyModel : public QSortFilterProxyModel
{
  BrowseProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {}

private:
  virtual bool filterAcceptsColumn(int source_column, const QModelIndex & /*source_parent*/) const
  {
    return source_column == 0; //only show 'Name' column inside the target QFileSystemModel
  }
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
  {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      if (section == 0)
        return "Browse";

    return QVariant();
  }
};

//
// proxy model for our TreeModel (filtering support)
//
struct CatalogProxyModel : public QSortFilterProxyModel
{
  CatalogProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
  {
      //auto enable parent items (too) for tree items that match our filter
      setRecursiveFilteringEnabled(true);
  }

  void setFilenameFilterExpression(QString filename)
  {
      m_filenameFilterExpr = filename;
      //Q_ASSERT(!m_filenameFilterExpr.isEmpty());

      m_treeModel = dynamic_cast<const TreeModel*>(sourceModel());
      Q_ASSERT(m_treeModel);

      invalidateFilter(); //reload ui according to the updated filter
  }

  virtual bool filterAcceptsColumn(int source_column, const QModelIndex & /*source_parent*/) const
  {
    return source_column == 0; //only show 'Name' column inside the target QFileSystemModel
  }

  virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
  {
      if (m_filenameFilterExpr.isEmpty())
          return true;

      const QModelIndex & itemIndex = m_treeModel->index(source_row, 0, source_parent);
      TreeModelItem* pItem = m_treeModel->item(itemIndex);
      Q_ASSERT(pItem);

      return pItem->filenameContains(m_filenameFilterExpr);
  }

private:
  QString m_filenameFilterExpr; //a tree item is only shown if its file path contains this one
  const TreeModel* m_treeModel = nullptr;
};

/*
 * application's main window
 */
MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  ui->splitter->setStretchFactor(1, 2);

  //model for left panel: "browse" treeview
  {
      QFileSystemModel *browseModel = new QFileSystemModel();
      browseModel->setRootPath(QDir::currentPath());
      //model->setFilter(QDir::AllDirs | QDir::Files);
      browseModel->setNameFilters(QStringList() << "*"+LOCATEDB_FILEEXT);
      browseModel->setNameFilterDisables(false);

      BrowseProxyModel *browseProxyModel = new BrowseProxyModel(this);
      browseProxyModel->setSourceModel(browseModel);

      ui->treeViewCollection->setModel(browseProxyModel);
  }

  //model for right panel: "catalog contents" treeview
  {
      TreeModel* catalogModel = new TreeModel();

      CatalogProxyModel *catalogProxyModel = new CatalogProxyModel(this);
      catalogProxyModel->setSourceModel(catalogModel);

      ui->treeViewContents->setModel(catalogProxyModel);
  }

  //selection changes shall trigger a slot
  connect(ui->treeViewCollection->selectionModel(), &QItemSelectionModel::selectionChanged,
          this,                                     &MainWindow::onCollectionChanged);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_actionOpenCatalog_triggered()
{
  const QFileInfo & current = getBrowsed();

  const QString & catalogFile = QFileDialog::getOpenFileName(this, tr("Load Catalog"), current.absolutePath(), "*"+LOCATEDB_FILEEXT);

  QApplication::setOverrideCursor(Qt::WaitCursor);

  const QStringList & paths = loadCatalog(catalogFile);

  showCatalogContents(catalogFile, paths);

  QApplication::restoreOverrideCursor();
}

void MainWindow::on_actionNewCatalog_triggered()
{
  const QFileInfo & current = getBrowsed();

  const QString & catalogFile = QFileDialog::getSaveFileName(this, tr("Save Catalog (%1) as").arg(current.absolutePath()), current.absolutePath(), QString("Catalog Files (*%1)").arg(LOCATEDB_FILEEXT));

  QApplication::setOverrideCursor(Qt::WaitCursor);

  saveCatalog(catalogFile, current.absolutePath());

  QApplication::restoreOverrideCursor();
}

void MainWindow::on_actionOptions_triggered()
{
  OptionsDialog dlg(this);
  if (dlg.exec() == QDialog::Accepted)
  {
    //save preferences
  }
}

void MainWindow::on_actionContents_triggered()
{
  //help contents
}

void MainWindow::on_actionAbout_triggered()
{
  //about
}

void MainWindow::on_lineEditFilterCollection_returnPressed()
{
  //filter collection directories
  BrowseProxyModel* pProxyModel = dynamic_cast<BrowseProxyModel*>(ui->treeViewCollection->model());
  Q_ASSERT(pProxyModel);

  QFileSystemModel* pModel = dynamic_cast<QFileSystemModel*>(pProxyModel->sourceModel());
  Q_ASSERT(pModel);

  QStringList flt;
  flt << ui->lineEditFilterCollection->text();
  pModel->setNameFilters(flt); //customize filtering
}

void MainWindow::on_lineEditFilterContents_returnPressed()
{
    //filter collection directories
    CatalogProxyModel* pProxyModel = dynamic_cast<CatalogProxyModel*>(ui->treeViewContents->model());
    Q_ASSERT(pProxyModel);

    //TreeModel* pModel = dynamic_cast<TreeModel*>(pProxyModel->sourceModel());
    //Q_ASSERT(pModel);

    QString expr = ui->lineEditFilterContents->text();
    pProxyModel->setFilenameFilterExpression(expr);  //customize filtering
}

QFileInfo MainWindow::getBrowsed()
{
    const QModelIndex index = ui->treeViewCollection->selectionModel()->currentIndex();

    BrowseProxyModel* pProxyModel = dynamic_cast<BrowseProxyModel*>(ui->treeViewCollection->model());
    Q_ASSERT(pProxyModel);

    QFileSystemModel* pModel = dynamic_cast<QFileSystemModel*>(pProxyModel->sourceModel());
    Q_ASSERT(pModel);

    const QFileInfo & fi = pModel->fileInfo(pProxyModel->mapToSource(index));

    return fi;
}

void MainWindow::showCatalogContents(QString catalogFile, const QStringList & catalogPaths)
{
    setWindowTitle(catalogFile);

    CatalogProxyModel* pProxyModel = dynamic_cast<CatalogProxyModel*>(ui->treeViewContents->model());
    Q_ASSERT(pProxyModel);

    TreeModel* pModel = dynamic_cast<TreeModel*>(pProxyModel->sourceModel());
    Q_ASSERT(pModel);

    pModel->clear();

    if (!catalogPaths.empty())
    {
      TreeModelItem* pNewItem = new TreeModelItem(catalogFile);
      buildTreePath(pNewItem, catalogPaths);

      pModel->add(pNewItem);
    }
}

void MainWindow::onCollectionChanged(const QItemSelection & /*newSelection*/, const QItemSelection & /*oldSelection*/)
{
  //get the text of the selected item
//  const QModelIndex index = ui->treeViewCollection->selectionModel()->currentIndex();
//  const QString & selectedText = index.data(Qt::DisplayRole).toString();

//  //find out the hierarchy level of the selected item
//  int hierarchyLevel = 1;
//  QModelIndex seekRoot = index;
//  while(seekRoot.parent() != QModelIndex())
//  {
//     seekRoot = seekRoot.parent();
//     hierarchyLevel++;
//  }

 const QFileInfo & fi = getBrowsed();

 const QString & filepath = fi.absoluteFilePath();

 QApplication::setOverrideCursor(Qt::WaitCursor);

 const QStringList & paths = filepath.endsWith(LOCATEDB_FILEEXT) ? loadCatalog(filepath) : QStringList();

 //QString showString = QString("%1 / %2 entries / Level %3").arg(selectedText).arg(paths.size()).arg(hierarchyLevel);
 //setWindowTitle(showString);

 showCatalogContents(filepath, paths);

 QApplication::restoreOverrideCursor();
}
