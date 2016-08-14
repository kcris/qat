#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileSystemModel>
#include "TreeModel.h"
#include "OptionsDialog.h"
#include "Catalog.h"

//
//proxy model that hides some columns inside QFileSystemModel
//
struct FSMProxy : public QSortFilterProxyModel
{
  FSMProxy(QObject *parent) : QSortFilterProxyModel(parent) {}

private:
  virtual bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
  {
    return source_column == 0; //only show 'Name' column inside the target QFileSystemModel
  }
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const
  {
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
      if (section == 0)
        return "Collection";

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
  model->setNameFilters(QStringList() << "*.db");
  model->setNameFilterDisables(false);

  FSMProxy *proxyModel = new FSMProxy(this);
  proxyModel->setSourceModel(model);
  ui->treeViewCollection->setModel(proxyModel);

  //setup catalog treeview
  TreeModel* colModel = new TreeModel();
  ui->treeViewContents->setModel(colModel);


  //selection changes shall trigger a slot
  connect(ui->treeViewCollection->selectionModel(), SIGNAL(selectionChanged (const QItemSelection &, const QItemSelection &)),
          this,                                     SLOT(onCollectionChanged(const QItemSelection &, const QItemSelection &)));
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_actionOpen_Collection_triggered()
{
  const QString & dir = QFileDialog::getExistingDirectory(this, tr("Open Collection"), QDir::currentPath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

  QMessageBox::information(this, "Directory", dir);
}

void MainWindow::on_actionAdd_Catalog_triggered()
{
  const QString & fileName = QFileDialog::getSaveFileName(this, tr("Create Catalog"), QDir::currentPath(), tr("Catalog Files (*.db)"));

  QMessageBox::information(this, "File", fileName);
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

void MainWindow::onCollectionChanged(const QItemSelection & /*newSelection*/, const QItemSelection & /*oldSelection*/)
{
  //get the text of the selected item
 const QModelIndex index = ui->treeViewCollection->selectionModel()->currentIndex();
 const QString & selectedText = index.data(Qt::DisplayRole).toString();

 //find out the hierarchy level of the selected item
 int hierarchyLevel = 1;
 QModelIndex seekRoot = index;
 while(seekRoot.parent() != QModelIndex())
 {
     seekRoot = seekRoot.parent();
     hierarchyLevel++;
 }



 FSMProxy* pProxyModel = dynamic_cast<FSMProxy*>(ui->treeViewCollection->model());
 Q_ASSERT(pProxyModel);
 QFileSystemModel* pModel = dynamic_cast<QFileSystemModel*>(pProxyModel->sourceModel());
 Q_ASSERT(pModel);
 const QFileInfo & fi = pModel->fileInfo(pProxyModel->mapToSource(index));

 const QString & filepath = fi.absoluteFilePath();

//#ifndef QT_NO_DEBUG
//  QStringList paths;
//  paths.append("x1/x2/x3");
//  paths.append("x1/x2/x4");
//  paths.append("x1/x5");
//#else
 const QStringList & paths = loadCatalog(filepath);
//#endif

 QString showString = QString("%1 / %2 entries / Level %3").arg(selectedText).arg(paths.size()).arg(hierarchyLevel);
 setWindowTitle(showString);


 TreeModel* model = dynamic_cast<TreeModel*>(ui->treeViewContents->model());
 model->clear();

 if (!paths.empty())
 {
   //model->add(selectedText);

   TreeModelItem* pNewItem = new TreeModelItem(filepath);
   buildTreePath(pNewItem, paths);
   model->add(pNewItem);
 }
}
