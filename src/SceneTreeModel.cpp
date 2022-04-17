#include "SceneTreeModel.h"
#include <QGridLayout>
#include <QStringListModel>
#include <iostream>
#include <QVariant>
#include <QVector>
#include <QFile>
#include "SceneObject.h"


bool SceneTreeModel::insertSceneObject(int _pos, std::shared_ptr<SceneObject> _object, const QModelIndex &_parent)
{
    std::shared_ptr<SceneObject> parentObject = getItem(_parent);
    if (!parentObject)
        return false;
    //std::cout<<"\nBegin insret rows...";
    beginInsertRows(_parent, _pos, _pos);
    SceneObject::setParentChild(parentObject, _object);
    //std::cout<<"\nFinished getting parent child...";
    endInsertRows();
    
    return true;
}

bool SceneTreeModel::removeSceneObject(const QModelIndex &_obj)
{
    auto object = getItem(_obj);

    auto parentObj = object->getParent();
    std::cout<<"parent is "<<parentObj->getName()<<std::endl;

    int pos = parentObj->indexOf(object);
    std::cout<<"pos is "<<pos<<std::endl;

    auto parentIndex = parent(_obj);

    beginRemoveRows(parentIndex, pos, pos);
    parentObj->removeChild(pos);
    endRemoveRows();

    return true;
}

SceneTreeModel::~SceneTreeModel()
{
}