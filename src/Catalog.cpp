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

static QStringList runCmd(QStringList cmdLine)
{
    if (cmdLine.isEmpty())
        return QStringList();

    const QString program = cmdLine.front();
    cmdLine.pop_front();
    const QStringList & arguments = cmdLine;

    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    process.start(program, arguments);

    if(!process.waitForStarted(10000))
      throw std::runtime_error("cannot start process");

    if(!process.waitForFinished(120000))
      throw std::runtime_error("cannot finish process");

    if (process.exitStatus() != QProcess::NormalExit)
        throw std::runtime_error("crashed process: co" + std::to_string(process.exitCode()));

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
    QStringList cmdLine;

    cmdLine << "wsl"
            << "wslpath"
            << "-a" //get absolute path
            << "-u" << path
    ;

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

  QStringList cmdLine;
  cmdLine

#ifdef Q_OS_WIN
          << "wsl"
#endif
          << "updatedb"
          << "--require-visibility" << "0" //db available to all users
          << "-U" << theInputDir
          << "-o" << theLocateDb
  ;

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

  QStringList cmdLine;
  cmdLine

#ifdef Q_OS_WIN
          << "wsl"
#endif
          << "locate"
          << "" //search *all* files inside our database
          << "-d" << theLocateDb
  ;

  return runCmd(cmdLine);
}

//locate -S = show statistics
