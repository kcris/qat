#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QItemSelection>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  void on_actionOpen_Collection_triggered();
  void on_actionAdd_Catalog_triggered();
  void on_actionOptions_triggered();
  void on_actionContents_triggered();
  void on_actionAbout_triggered();

  void on_lineEditFilterCollection_returnPressed();
  void on_lineEditFilterContents_returnPressed();

private slots:
  void onCollectionChanged(const QItemSelection &, const QItemSelection &);

private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
