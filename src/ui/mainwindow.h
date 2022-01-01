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
  void on_lineEditFilterCatalogContents_textChanged(const QString &arg1);

private:
  void loadCatalog(const QString & catalogFile, const QString & expr);
  void loadCatalogModels(QString catalogFile, QString expr, const QStringList & catalogPaths);

  QFileInfo getBrowsed();
  void onCurrentBrowsedChanged(const QItemSelection &, const QItemSelection &);

private:
  Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
