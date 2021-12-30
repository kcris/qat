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

static QStringList runCmd(const QString & cmdLine)
{
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(cmdLine, QIODevice::ReadWrite);
    if(!process.waitForStarted())
      throw std::runtime_error("cannot start process");

    if(!process.waitForFinished())
      throw std::runtime_error("cannot finish process");

    const QByteArray output(process.readAllStandardOutput());
    return QString(output).split("\n");
}

static QString getPath(QString path)
{
#ifdef Q_OS_WIN
    QString cmdLine = QString("wsl wslpath -a -u %1").arg(path);

    QStringList lines = runCmd(cmdLine);

    return lines[0];
#else
    return path;
#endif
}

//
// generate .db catalog file
//
// updatedb -U inputDir -o outputFile.db
//
QStringList saveCatalog(const QString &dirName, const QString &catalogFile)
{
  QString theInputDir = getPath(dirName);
  QString theLocateDb = getPath(catalogFile);

  QString cmdLine = QString("wsl updatedb --require-visibility 0 -U %1 -o %2")
          .arg(theInputDir)
          .arg(theLocateDb);
  return runCmd(cmdLine);
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

  QString theLocateDb = getPath(catalogFile);

  QString cmdLine = QString("wsl locate -P * -d %1")
          .arg(theLocateDb);
  return runCmd(cmdLine);
}
