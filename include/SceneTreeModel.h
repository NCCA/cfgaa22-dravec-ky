#ifndef SCENETREEMODEL_H_
#define SCENETREEMODEL_H_

#include <QTreeView>
#include <QStringListModel>
#include <QAbstractItemModel>
#include "SceneObject.h"

class IndexWrapper
{
  public:
    std::shared_ptr<SceneObject> object;
    explicit IndexWrapper(std::shared_ptr<SceneObject> _object) {object = _object;}
    ~IndexWrapper() {}
    QVector<QVariant> q_data;
};

//https://doc.qt.io/qt-5/qtwidgets-itemviews-simpletreemodel-example.html
class SceneTreeModel: public QAbstractItemModel
{
    Q_OBJECT
  public:
    explicit SceneTreeModel(const QStringList &headers, const QString &data, QObject *parent = nullptr);
    ~SceneTreeModel() {}

    bool setObject(const QModelIndex &index, std::shared_ptr<SceneObject> _object);

    QVariant data(const QModelIndex &index, int role) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QVariant headerData(int section, Qt::Orientation orientation,
                          int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override
          {Q_UNUSED(parent); return 1;}

    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;
    bool setHeaderData(int section, Qt::Orientation orientation,
                       const QVariant &value, int role = Qt::EditRole) override;
    
    bool insertColumns(int position, int columns,
                        const QModelIndex &parent = QModelIndex()) override
                        {return false;}
    bool removeColumns(int position, int columns,
                        const QModelIndex &parent = QModelIndex()) override
                        {return false;}

    bool insertRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int position, int rows,
                    const QModelIndex &parent = QModelIndex()) override;

    std::shared_ptr<SceneObject> getRootObject() const {return m_rootObject;}

  private:
    std::shared_ptr<SceneObject> getItem(const QModelIndex &index) const;
    
    std::shared_ptr<SceneObject> m_rootObject;

};

#endif