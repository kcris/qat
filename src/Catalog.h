#ifndef CATALOG_H
#define CATALOG_H

#include <QString>

//write catalog file (will contain: all paths inside the specified dir)
QStringList saveCatalog(const QString & catalogFile, const QString & dirName);

//read catalog file (return a list of paths)
QStringList loadCatalog(const QString & catalogFile);

#endif // CATALOG_H
