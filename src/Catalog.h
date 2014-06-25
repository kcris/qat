#ifndef CATALOG_H
#define CATALOG_H

#include "Tree.hpp"

#include <QString>

//write catalog file
bool saveCatalog(const QString & dirName, const QString & fileName);

//load catalog file (return a list of paths)
QStringList loadCatalog(const QString & fileName);

//build tree from list of paths
void buildTreePath(TreeNode<QString>* pParent, const QStringList & paths);

#endif // CATALOG_H
