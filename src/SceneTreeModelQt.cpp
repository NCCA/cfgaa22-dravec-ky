#include "SceneTreeModel.h"
#include <QGridLayout>
#include <QStringListModel>
#include <iostream>
#include <QVariant>
#include <QVector>
#include <QFile>
#include "SceneObject.h"

//Override functions for QModel

SceneTreeModel::SceneTreeModel(const QStringList &headers, const QString &data, QObject *parent)
    : QAbstractItemModel(parent)
{
    // QVector<QVariant> rootData;
    // for (const QString &header : headers)
    //     rootData << header;

    m_rootObject = std::make_shared<SceneObject>();
    m_rootObject->setData("root");
    m_rootObject->setName("root");
    //setupModelData(data.split('\n'), std::move(m_rootObject));
}

std::shared_ptr<SceneObject> SceneTreeModel::getItem(const QModelIndex &index) const
{

    if (index.isValid()) {
        //std::cout<<"Index is valid.\n";
        
        auto wrapper = static_cast<IndexWrapper*>(index.internalPointer());
        auto object_ref = wrapper->object;
        auto object = std::move(object_ref);

        if (object)
        {
            return object;
        }
    }
    //std::cout<<"Index isn't valid, returning root\n";
    
    return m_rootObject;
}

Qt::ItemFlags SceneTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}

QModelIndex SceneTreeModel::index(int index, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    std::shared_ptr<SceneObject> parentObject = getItem(parent);
    if (!parentObject)
        return QModelIndex();

    std::shared_ptr<SceneObject> childObject = parentObject->getChild(index);
    auto wrappedChild = new IndexWrapper(childObject);
    if (childObject)
        return createIndex(index, column, wrappedChild);

    return QModelIndex();
}

bool SceneTreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    std::shared_ptr<SceneObject> parentObject = getItem(parent);
    if (!parentObject)
        return false;
    //std::cout<<"\nBegin insret rows...";
    beginInsertRows(parent, position, position + rows - 1);
    SceneObject::setParentChild(parentObject, std::make_shared<SceneObject>());
    //std::cout<<"\nFinished getting parent child...";
    endInsertRows();

    return true;
}

bool SceneTreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    std::shared_ptr<SceneObject> parentObject = getItem(parent);
    if (!parentObject)
        return false;

    beginRemoveRows(parent, position, position + rows - 1);
    parentObject->removeChild(position);
    endRemoveRows();

    return true;
}

int SceneTreeModel::rowCount(const QModelIndex &parent) const
{
    //std::cout<<"Checking row count...\n";
    if (parent.isValid() && parent.column() > 0)
        return 0;

    auto parentObject = getItem(parent);
    //std::cout << parentObject->getName() << std::endl;

    return parentObject ? parentObject->childCount() : 0;
}

QModelIndex SceneTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();
  
    std::shared_ptr<SceneObject> childObject = getItem(index);
    std::shared_ptr<SceneObject> parentObject =  childObject ? childObject->getParent() : nullptr;

    if (parentObject == m_rootObject || !parentObject)
        return QModelIndex();

    auto parentIndex = parentObject->getParent()->indexOf(parentObject);

    auto wrappedParent = new IndexWrapper(parentObject);
    return createIndex(parentIndex, 0, wrappedParent);
}

QVariant SceneTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    std::shared_ptr<SceneObject> object = getItem(index);

    return object->data();
}

bool SceneTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;
    std::shared_ptr<SceneObject> object = getItem(index);
    bool result = object->setData(value);

    if (result)
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

    return result;
}

QVariant SceneTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_rootObject->data();

    return QVariant();
}

bool SceneTreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    const bool result = m_rootObject->setData(value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}