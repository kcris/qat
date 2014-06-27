#ifndef CATALOG_H
#define CATALOG_H

#include <QString>

//write catalog file
bool saveCatalog(const QString & dirName, const QString & fileName);

//load catalog file (return a list of paths)
QStringList loadCatalog(const QString & fileName);

#endif // CATALOG_H
