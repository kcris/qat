#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <QAbstractItemModel>

#include "TreeModelItem.h"

class TreeModel : public QAbstractItemModel
{
  Q_OBJECT
public:
  TreeModel(QObject* parent = 0);
  virtual ~TreeModel();

  void clear();

  void add(const QString & data); //toplevel item
  void add(TreeModelItem* pNewItem); //toplevel item

  TreeModelItem* item(const QModelIndex &index) const;

private:
  virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
  virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
  virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
  virtual QModelIndex parent(const QModelIndex &child) const;
  virtual Qt::ItemFlags flags(const QModelIndex &index) const;
  virtual QVariant data(const QModelIndex &index, int role) const;
  virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

private:
  struct Private;
  Private* m_priv;
};

#endif //TREEMODEL_H
