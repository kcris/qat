#ifndef CATALOG_H
#define CATALOG_H

#include <QString>

//write catalog file
bool saveCatalog(const QString & dirName, const QString & catalogFile);

//load catalog file (return a list of paths)
QStringList loadCatalog(const QString & catalogFile);

#endif // CATALOG_H
