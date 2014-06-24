#ifndef CATALOG_H
#define CATALOG_H

#include <QString>
#include <QLinkedList>

/*
 * generic tree structure - can hold the filesystem hierarchy inside a catalog
 */
template<typename T>
class TreeNode;

template<typename T>
struct Visitor
{
  virtual void visitNode(TreeNode<T> * node) = 0;
};

template<typename T>
struct TreeNode
{
  QLinkedList<TreeNode*> children;
  const T data;

  TreeNode(const T & d)
    : data(d)
  {
  }

  void accept(Visitor<T> * visitor)
  {
    visitor->visitNode(this);
  }

  TreeNode* child(const T & data)
  {
    foreach (TreeNode * child, children )
      if (child->data == data)
        return child;

    TreeNode* childNode = new TreeNode(data);
    children.append(childNode);
    return childNode;
  }
};

//write catalog file
bool saveCatalog(const QString & dirName, const QString & fileName);

//load catalog file (return a list of paths)
QStringList loadCatalog(const QString & fileName);

//build tree from list of paths
TreeNode<QString>* buildTreePath(const QStringList & paths);

#endif // CATALOG_H
