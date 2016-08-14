#include "Catalog.h"

#include <QStringList>
#include <QProcess>
#include <QFile>

//#include <iostream>

//class PrintIndentedVisitor : public Visitor<QString>
//{
//  int m_indent;
//public:
//  PrintIndentedVisitor(int i)
//    : m_indent(i)
//  {
//  }

//  virtual void visitNode(TreeNode<QString> * node)
//  {
//    std::cout << std::string(m_indent, ' ') << node->data.toStdString() << std::endl;

//    //visit children
//    m_indent += 2;
//    foreach (TreeNode<QString> * child, node->children)
//      child->accept(this);
//    m_indent -= 2;
//  }
//};

static const QString path = ""; //d:/bin/cygwin64/bin"; //TODO - use QSettings to store this

//
// generate .db catalog file
//
// updatedb -U inputDir -o outputFile.db
//
bool saveCatalog(const QString &dirName, const QString &catalogFile)
{
  QString updatedbCmd = "updatedb";
  if (!path.isEmpty())
    updatedbCmd = path + "/" + updatedbCmd;

  QProcess process;
  //process.setProcessChannelMode(QProcess::MergedChannels);
  process.start(QString("%1 -U %2 -o %3").arg(updatedbCmd).arg(dirName).arg(catalogFile), QIODevice::ReadWrite);
  if(!process.waitForStarted())
    return false;
  if(!process.waitForFinished())
    return false;

  return true;
}

//#define USE_READLINE

//
// print contents of .db catalog file
//
// locate -d inputFile.db -P *
//
QStringList loadCatalog(const QString & catalogFile)
{
  if (!QFile::exists(catalogFile))
    return QStringList();

  QString locateCmd = "locate";
  if (!path.isEmpty())
    locateCmd = path + "/" + locateCmd;

  QProcess process;
  process.setProcessChannelMode(QProcess::MergedChannels);
  process.start(QString("%1 -d %2 -P *").arg(locateCmd).arg(catalogFile), QIODevice::ReadWrite);
  if(!process.waitForStarted())
    return QStringList();

  QStringList items;

#ifdef USE_READLINE
  while(process.waitForReadyRead())
    items.append(process.readLine());
#endif

  if(!process.waitForFinished())
    return items;

#ifndef USE_READLINE
  const QByteArray output(process.readAllStandardOutput());
  items = QString(output).split("\n");
#endif

  return items;
}
