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


//
// generate .db catalog file
//
// updatedb -U dirName -o fileName
//
bool saveCatalog(const QString &dirName, const QString &fileName)
{
  QProcess process;
  //process.setProcessChannelMode(QProcess::MergedChannels);
  process.start(QString("updatedb -U %1 -o %2").arg(dirName).arg(fileName), QIODevice::ReadWrite);
  if(!process.waitForStarted())
    return false;
  if(!process.waitForFinished())
    return false;

  return true;
}


//
// print contents of .db catalog file
//
// locate -d fileName -P *
//
QStringList loadCatalog(const QString & fileName)
{
  if (!QFile::exists(fileName))
    return QStringList();

  QProcess process;
  process.setProcessChannelMode(QProcess::MergedChannels);
  process.start(QString("locate -d %1 -P *").arg(fileName), QIODevice::ReadWrite);
  if(!process.waitForStarted())
    return QStringList();

  QStringList items;

  while(process.waitForReadyRead())
    items.append(process.readLine());

  if(!process.waitForFinished())
    return QStringList();

  return items;
}


//void buildTreePath(TreeNode<QString>* pParent, const QStringList & paths)
//{
//  TreeNode<QString>* current = pParent;

//  foreach (const QString & path, paths)
//  {
//    TreeNode<QString> * node = current;

//    foreach (const QString & data, path.split("/"))
//    {
//      current = current->child(data);
//    }

//    current = node;
//  }

//  //pParent->accept(new PrintIndentedVisitor(0));
//}

