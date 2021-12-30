#ifndef TREEMODELITEM_H
#define TREEMODELITEM_H

#include <QtCore/QtGlobal>
#include <QtCore/QString>
#include <QtCore/QVariant>

#include <vector>

struct TreeModelItem
{
  TreeModelItem(const QString & data)
    : m_pParentItem(NULL)
    , m_data(data)
  {
  }

  virtual ~TreeModelItem()
  {
    removeChilds();
  }

  TreeModelItem* addChild(const QString & data)
  {
    for (std::vector<TreeModelItem*>::const_iterator it = m_vChildItems.begin(), itEnd = m_vChildItems.end(); it != itEnd; ++it)
      if ((*it)->m_data == data)
        return *it;

    TreeModelItem* pNewItem = new TreeModelItem(data);
    pNewItem->setParent(this);
    m_vChildItems.push_back(pNewItem);
    return pNewItem;
  }

  TreeModelItem* addChild(TreeModelItem* pNewItem)
  {
    for (std::vector<TreeModelItem*>::const_iterator it = m_vChildItems.begin(), itEnd = m_vChildItems.end(); it != itEnd; ++it)
      if ((*it)->m_data == pNewItem->m_data)
        return *it;

    pNewItem->setParent(this);
    m_vChildItems.push_back(pNewItem);
    return pNewItem;
  }

  void removeChilds()
  {
    for (std::vector<TreeModelItem*>::const_iterator it = m_vChildItems.begin(), itEnd = m_vChildItems.end(); it != itEnd; ++it)
      delete *it;
    m_vChildItems.clear();
  }

  void setParent(TreeModelItem* pParent)                    {m_pParentItem = pParent;}
  TreeModelItem* getParentItem()                            {return m_pParentItem;}
  const std::vector<TreeModelItem*> & getChildItems() const {return m_vChildItems;}

  virtual Qt::ItemFlags flags(int column) const
  {
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  }
  virtual QVariant data(int column, int role) const
  {
    if (role == Qt::DisplayRole)
      return m_data;

    return QVariant();
  }
  virtual bool setData(int column, const QVariant &value, int role)
  {
    return false;
  }
private:
  TreeModelItem* m_pParentItem;
  std::vector<TreeModelItem*> m_vChildItems; //TODO: QLinkedList ?
  QString m_data;
};


#endif //TREEMODELITEM_H
