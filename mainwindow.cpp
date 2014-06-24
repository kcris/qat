#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QFileSystemModel>
#include "CollectionModel.h"
#include "OptionsDialog.h"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  //setup collection treeview
  QFileSystemModel *model = new QFileSystemModel();
  model->setRootPath(QDir::currentPath());
  //model->setFilter(QDir::AllDirs | QDir::Files);
  model->setNameFilters(QStringList() << "*.db");
  ui->treeViewCollection->setModel(model);

  //setup catalog treeview
  //CollectionModel* colModel = new CollectionModel();
  //ui->treeViewContents->setModel(colModel);


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
  QFileSystemModel* pModel = dynamic_cast<QFileSystemModel*>(ui->treeViewCollection->model());
  Q_ASSERT(pModel);
  pModel->setNameFilters(QStringList() << ui->lineEditFilterCollection->text());
}

void MainWindow::on_lineEditFilterContents_returnPressed()
{
  //filter catalogs contents
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

 QString showString = QString("%1, Level %2").arg(selectedText).arg(hierarchyLevel);
 setWindowTitle(showString);

 const QStringList & paths = loadCatalog(selectedText);

 // QStringList paths;
 // paths.append("x1/x2/x3");
 // paths.append("x1/x2/x4");
 // paths.append("x1/x5");

 if (!paths.empty())
 {
   TreeNode<QString>* pRoot = buildTreePath(paths);

   ui->treeViewContents; //must display pRoot

   delete pRoot;
 }
}
