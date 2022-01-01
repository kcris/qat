#ifndef CATALOG_H
#define CATALOG_H

#include <QString>

//write catalog file (will contain: all paths inside the specified dir)
QStringList saveCatalog(const QString & catalogFile, const QString & dirName);

//load catalog file (return a list of paths the locate expr)
QStringList loadCatalog(const QString & catalogFile, const QString & expr = QString(""));

#endif // CATALOG_H
