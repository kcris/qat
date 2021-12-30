#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileSystemModel>
#include "TreeModel.h"
#include "OptionsDialog.h"
#include "Catalog.h"

static const QString LOCATEDB_FILEEXT = ".db";

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
//proxy model that hides some columns inside QFileSystemModel
//
struct FSMProxy : public QSortFilterProxyModel
{
  FSMProxy(QObject *parent) : QSortFilterProxyModel(parent) {}

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


/*
 * application's main window
 */
MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  ui->splitter->setStretchFactor(1, 2);

  //setup collection treeview
  QFileSystemModel *model = new QFileSystemModel();
  model->setRootPath(QDir::currentPath());
  //model->setFilter(QDir::AllDirs | QDir::Files);
  model->setNameFilters(QStringList() << "*"+LOCATEDB_FILEEXT);
  model->setNameFilterDisables(false);

  FSMProxy *proxyModel = new FSMProxy(this);
  proxyModel->setSourceModel(model);
  ui->treeViewCollection->setModel(proxyModel);

  //setup catalog treeview
  TreeModel* colModel = new TreeModel();
  ui->treeViewContents->setModel(colModel);


  //selection changes shall trigger a slot
  connect(ui->treeViewCollection->selectionModel(), &QItemSelectionModel::selectionChanged,
          this,                                     &MainWindow::onCollectionChanged);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_actionOpen_Collection_triggered()
{
  const QFileInfo & fi = getBrowsed();

  const QString & catalogFile = QFileDialog::getOpenFileName(this, tr("Load Catalog"), fi.absolutePath(), "*"+LOCATEDB_FILEEXT);

  const QStringList & paths = loadCatalog(catalogFile);

  showCatalogContents(catalogFile, paths);
}

void MainWindow::on_actionAdd_Catalog_triggered()
{
  const QFileInfo & fi = getBrowsed();

  const QString & fileName = QFileDialog::getSaveFileName(this, tr("Save Catalog"), fi.absolutePath(), QString("Catalog Files (*%1)").arg(LOCATEDB_FILEEXT));

  saveCatalog(fi.absolutePath(), fileName);
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
  FSMProxy* pProxyModel = dynamic_cast<FSMProxy*>(ui->treeViewCollection->model());
  Q_ASSERT(pProxyModel);
  QFileSystemModel* pModel = dynamic_cast<QFileSystemModel*>(pProxyModel->sourceModel());
  Q_ASSERT(pModel);
  pModel->setNameFilters(QStringList() << ui->lineEditFilterCollection->text());
}

void MainWindow::on_lineEditFilterContents_returnPressed()
{
  //filter catalogs contents
}

QFileInfo MainWindow::getBrowsed()
{
    const QModelIndex index = ui->treeViewCollection->selectionModel()->currentIndex();

    FSMProxy* pProxyModel = dynamic_cast<FSMProxy*>(ui->treeViewCollection->model());
    Q_ASSERT(pProxyModel);

    QFileSystemModel* pModel = dynamic_cast<QFileSystemModel*>(pProxyModel->sourceModel());
    Q_ASSERT(pModel);

    const QFileInfo & fi = pModel->fileInfo(pProxyModel->mapToSource(index));

    return fi;
}

void MainWindow::showCatalogContents(QString catalogFile, const QStringList & catalogPaths)
{
    setWindowTitle(catalogFile);

    TreeModel* model = dynamic_cast<TreeModel*>(ui->treeViewContents->model());
    model->clear();

    if (!catalogPaths.empty())
    {
      TreeModelItem* pNewItem = new TreeModelItem(catalogFile);
      buildTreePath(pNewItem, catalogPaths);
      model->add(pNewItem);
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

 QApplication::setOverrideCursor(Qt::WaitCursor);

 const QFileInfo & fi = getBrowsed();

 const QString & filepath = fi.absoluteFilePath();

 const QStringList & paths = filepath.endsWith(LOCATEDB_FILEEXT) ? loadCatalog(filepath) : QStringList();

 //QString showString = QString("%1 / %2 entries / Level %3").arg(selectedText).arg(paths.size()).arg(hierarchyLevel);
 //setWindowTitle(showString);

 showCatalogContents(filepath, paths);

 QApplication::restoreOverrideCursor();
}
