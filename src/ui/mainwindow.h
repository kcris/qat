#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QFileInfo>
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
  void on_actionOpenCatalog_triggered();
  void on_actionNewCatalog_triggered();
  void on_actionOptions_triggered();
  void on_actionContents_triggered();
  void on_actionAbout_triggered();
  void on_actionBrowseList_triggered();
  void on_actionContentsList_triggered();

  void on_lineEditFilterBrowse_returnPressed();
  void on_lineEditFilterCatalogContents_returnPressed();

  void onCurrentDatabaseChanged(const QItemSelection &, const QItemSelection &);

private:
  QFileInfo getBrowsed();
  void showCatalogContents(QString catalogFile, const QStringList & catalogPaths);

private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
