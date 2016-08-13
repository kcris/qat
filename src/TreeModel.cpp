#include "TreeModel.h"

enum enColumns
{
  enColFilename,
  enColLast
};

const char* headerColumns[enColLast] =
{
  "File"
};

/**
 * struct TreeModel::Private
 */
struct TreeModel::Private
{
  Private() : m_pRootItem(NULL) {}

  TreeModelItem* m_pRootItem;
};


/**
 * class TreeModel
 */
TreeModel::TreeModel(QObject* parent)
  : QAbstractItemModel(parent)
  , m_priv(new Private())
{
  beginResetModel();
  m_priv->m_pRootItem = new TreeModelItem("/");
  endResetModel();
}

TreeModel::~TreeModel()
{
  clear();
  delete m_priv->m_pRootItem;
  delete m_priv;
}

void TreeModel::clear()
{
  beginResetModel();

  m_priv->m_pRootItem->removeChilds();

  endResetModel();
}

void TreeModel::add(const QString & data)
{
  beginResetModel();

  m_priv->m_pRootItem->addChild(data);

  endResetModel();
}

void TreeModel::add(TreeModelItem* pNewItem)
{
  beginResetModel();

  m_priv->m_pRootItem->addChild(pNewItem);

  endResetModel();
}

TreeModelItem* TreeModel::item(const QModelIndex &index) const
{
  Q_ASSERT(index.model() == this);
  if(!index.isValid())
    return NULL;

  TreeModelItem* pMyItem = reinterpret_cast<TreeModelItem*>(index.internalPointer());
  Q_ASSERT(pMyItem);
  return pMyItem;
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole && 0 <= section && section < columnCount())
    return headerColumns[section];
  return QVariant();
}

int TreeModel::columnCount(const QModelIndex &/*parent*/) const
{
  return enColLast;
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
  if (!m_priv->m_pRootItem)
    return 0;

  TreeModelItem* pParentItem = !parent.isValid() ? m_priv->m_pRootItem : item(parent); //toplevel shows contents of (invisible) root
  Q_ASSERT(pParentItem);

  return pParentItem ? pParentItem->getChildItems().size() : 0;
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  TreeModelItem* pParentItem = !parent.isValid() ? m_priv->m_pRootItem : item(parent);
  Q_ASSERT(pParentItem);
  
  TreeModelItem* pMyItem = pParentItem->getChildItems().at(row);
  return createIndex(row, column, pMyItem); //pass my tree node as 'internalPointer'
}

QModelIndex TreeModel::parent(const QModelIndex &child) const
{
  if (!child.isValid())
    return QModelIndex();

  //find parent based on child's 'internalPointer'
  TreeModelItem* pMyItem = item(child);

  TreeModelItem* pParentItem = pMyItem ? pMyItem->getParentItem() : m_priv->m_pRootItem;

  //find parent's row in grandparent
  if (!pParentItem)
    return QModelIndex(); //root node has no parent

  TreeModelItem* pGrandParentItem = pParentItem->getParentItem();
  if (!pGrandParentItem)
    return QModelIndex(); //direct child of the root

  const std::vector<TreeModelItem*> & parentSiblings = pGrandParentItem->getChildItems();
  std::vector<TreeModelItem*>::const_iterator itParent = std::find(parentSiblings.begin(), parentSiblings.end(), pParentItem);
  Q_ASSERT(itParent != parentSiblings.end());
  int row = std::distance(parentSiblings.begin(), itParent);

  return createIndex(row, 0, pParentItem); //ancestor of the root (not a direct child)
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
  TreeModelItem* pMyItem = item(index);
  if (!pMyItem)
    return QAbstractItemModel::flags(index);

  return pMyItem->flags(index.column());
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
  if(!index.isValid())
    return QVariant();

  TreeModelItem* pMyItem = item(index);
  if (!pMyItem)
    return QVariant();

  return pMyItem->data(index.column(), role);
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
  if(!index.isValid())
    return false;

  TreeModelItem* pMyItem = item(index);
  if (!pMyItem)
    return false;

  if (pMyItem->setData(index.column(), value, role))
  {
    emit dataChanged(index, index);

    return true;
  }

  return false;
}
