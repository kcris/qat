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

    if(!process.waitForStarted(10000))
      throw std::runtime_error("cannot start process");

    if(!process.waitForFinished(60000))
      throw std::runtime_error("cannot finish process");

    const QByteArray & output = process.readAllStandardOutput();

    return QString(output).split("\n");
}


/*
 * ---------------------------------------------------------------------------------------
 * WARNING: we rely on gnu tools like updatedb/locate; we will need WSL support on windows
 * ---------------------------------------------------------------------------------------
 */

//
// translate windows paths to linux ones
//
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
// create .db catalog file
//
QStringList saveCatalog(const QString &catalogFile, const QString &dirName)
{
  QString theInputDir = getPath(dirName);
  QString theLocateDb = getPath(catalogFile);

  const QString cmdLine =

#ifdef Q_OS_WIN
          QString("wsl ") +
#endif
          QString("updatedb --require-visibility 0 -U %1 -o %2")
            .arg(theInputDir)
            .arg(theLocateDb);

  return runCmd(cmdLine);
}

//
// load .db catalog file
//
QStringList loadCatalog(const QString & catalogFile)
{
  if (!QFile::exists(catalogFile))
    return QStringList();

  QString theLocateDb = getPath(catalogFile);

  const QString cmdLine =
#ifdef Q_OS_WIN
          QString("wsl ") +
#endif
          QString("locate -P * -d %1")
            .arg(theLocateDb);

  return runCmd(cmdLine);
}
