#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileSystemModel>
#include <QStringListModel>
#include "TreeModel.h"
#include "OptionsDialog.h"
#include "Catalog.h"

static const QString LOCATEDB_FILEEXT = QStringLiteral(".db");

void buildTreePath(MainWindow* pMainWindow, TreeModelItem* pParent, const QStringList & paths)
{
  //const int nTotalEntries = paths.size();

  TreeModelItem* current = pParent;

  //int n = 0;
  foreach (const QString & path, paths)
  {
    //pMainWindow->statusBar()->showMessage(QObject::tr("Loading entry %1/%2").arg(++n).arg(nTotalEntries));

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

      //a tree item is only shown if its file path contains this expr
      return pItem->filenameContains(m_filenameFilterExpr);
  }

private:
  QString m_filenameFilterExpr;
  const TreeModel* m_treeModel = nullptr;
};

struct CatalogListProxyModel : public QSortFilterProxyModel
{
    CatalogListProxyModel(QObject *parent) : QSortFilterProxyModel(parent) {}

    void setFilenameFilterExpression(QString filename)
    {
        m_filenameFilterExpr = filename;

        m_listModel = dynamic_cast<const QStringListModel*>(sourceModel());
        Q_ASSERT(m_listModel);

        invalidateFilter(); //reload ui according to the updated filter
    }

    virtual bool filterAcceptsRow(int source_row, const QModelIndex & /*source_parent*/) const
    {
        if (m_filenameFilterExpr.isEmpty())
            return true;

        const QStringList & stringList = m_listModel->stringList();
        Q_ASSERT(0 <= source_row && source_row < stringList.size());
        const QString & item = stringList.at(source_row);

        //a list item is only shown if its file path contains this expr
        return item.contains(m_filenameFilterExpr);
    }

private:
    QString m_filenameFilterExpr;
    const QStringListModel* m_listModel = nullptr;
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

      ui->treeViewBrowse->setModel(browseProxyModel); //treeview

      ui->listViewBrowse->setModel(browseProxyModel); //listview
  }

  //model for right panel: "catalog contents" treeview
  {
      TreeModel* catalogModel = new TreeModel();

      CatalogProxyModel *catalogProxyModel1 = new CatalogProxyModel(this);
      catalogProxyModel1->setSourceModel(catalogModel);

      ui->treeViewCatalogContents->setModel(catalogProxyModel1); //treeview



      QStringListModel* catalogListModel = new QStringListModel();

      CatalogListProxyModel *catalogProxyModel2 = new CatalogListProxyModel(this);
      catalogProxyModel2->setSourceModel(catalogListModel);

      ui->listViewCatalogContents->setModel(catalogProxyModel2); //listview
  }

  //selection changes shall trigger a slot
  connect(ui->treeViewBrowse->selectionModel(), &QItemSelectionModel::selectionChanged,
          this,                                 &MainWindow::onCurrentDatabaseChanged);
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

void MainWindow::on_lineEditFilterBrowse_returnPressed()
{
  //filter collection directories
  BrowseProxyModel* pProxyModel = dynamic_cast<BrowseProxyModel*>(ui->treeViewBrowse->model());
  Q_ASSERT(pProxyModel);

  QFileSystemModel* pModel = dynamic_cast<QFileSystemModel*>(pProxyModel->sourceModel());
  Q_ASSERT(pModel);

  QStringList flt;
  flt << ui->lineEditFilterBrowse->text();
  pModel->setNameFilters(flt); //customize filtering
}

void MainWindow::on_lineEditFilterCatalogContents_returnPressed()
{
    const QString expr = ui->lineEditFilterCatalogContents->text();

    //
    // tree view
    //
    CatalogProxyModel* pProxyModel1 = dynamic_cast<CatalogProxyModel*>(ui->treeViewCatalogContents->model());
    Q_ASSERT(pProxyModel1);

    //TreeModel* pModel = dynamic_cast<TreeModel*>(pProxyModel1->sourceModel());
    //Q_ASSERT(pModel);

    pProxyModel1->setFilenameFilterExpression(expr);  //customize filtering

    //
    // list view
    //
    CatalogListProxyModel* pProxyModel2 = dynamic_cast<CatalogListProxyModel*>(ui->listViewCatalogContents->model());
    Q_ASSERT(pProxyModel2);

    pProxyModel2->setFilenameFilterExpression(expr);
}

QFileInfo MainWindow::getBrowsed()
{
    const QModelIndex index = ui->treeViewBrowse->selectionModel()->currentIndex();

    BrowseProxyModel* pProxyModel = dynamic_cast<BrowseProxyModel*>(ui->treeViewBrowse->model());
    Q_ASSERT(pProxyModel);

    QFileSystemModel* pModel = dynamic_cast<QFileSystemModel*>(pProxyModel->sourceModel());
    Q_ASSERT(pModel);

    const QFileInfo & fi = pModel->fileInfo(pProxyModel->mapToSource(index));

    return fi;
}

void MainWindow::showCatalogContents(QString catalogFile, const QStringList & catalogPaths)
{
    setWindowTitle(QString("%1 (%2 entries)").arg(catalogFile).arg(catalogPaths.size()));

    //
    // treeview
    //
    CatalogProxyModel* pProxyModel1 = dynamic_cast<CatalogProxyModel*>(ui->treeViewCatalogContents->model());
    Q_ASSERT(pProxyModel1);

    TreeModel* pTreeModel = dynamic_cast<TreeModel*>(pProxyModel1->sourceModel());
    Q_ASSERT(pTreeModel);

    pTreeModel->clear();

    if (!catalogPaths.empty())
    {
      TreeModelItem* pNewItem = new TreeModelItem(catalogFile);
      buildTreePath(this, pNewItem, catalogPaths);

      pTreeModel->add(pNewItem);
    }

    //auto expand
    ui->treeViewCatalogContents->expandAll();


    //
    // list view
    //
    CatalogListProxyModel* pProxyModel2 = dynamic_cast<CatalogListProxyModel*>(ui->listViewCatalogContents->model());
    Q_ASSERT(pProxyModel2);

    QStringListModel* pListModel = dynamic_cast<QStringListModel*>(pProxyModel2->sourceModel());
    Q_ASSERT(pListModel);

    pListModel->setStringList(catalogPaths);
}

void MainWindow::onCurrentDatabaseChanged(const QItemSelection & /*newSelection*/, const QItemSelection & /*oldSelection*/)
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

 statusBar()->showMessage(tr("Opening catalog"));

 QApplication::setOverrideCursor(Qt::WaitCursor);

 const QStringList & paths = filepath.endsWith(LOCATEDB_FILEEXT) ? loadCatalog(filepath) : QStringList();

 //QString showString = QString("%1 / %2 entries / Level %3").arg(selectedText).arg(paths.size()).arg(hierarchyLevel);
 //setWindowTitle(showString);

 statusBar()->showMessage(tr("Loading catalog"));

 showCatalogContents(filepath, paths);

 QApplication::restoreOverrideCursor();
}

void MainWindow::on_actionBrowseList_triggered()
{
    bool useBrowseList = ui->actionBrowseList->isChecked();

    ui->stackedWidgetBrowse->setCurrentIndex(useBrowseList ? 1 : 0);
}

void MainWindow::on_actionContentsList_triggered()
{
    bool useContentsList = ui->actionContentsList->isChecked();

    ui->stackedWidgetContents->setCurrentIndex(useContentsList ? 1 : 0);
}
