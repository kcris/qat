#ifndef TREE_HPP
#define TREE_HPP

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

#endif // TREE_HPP
