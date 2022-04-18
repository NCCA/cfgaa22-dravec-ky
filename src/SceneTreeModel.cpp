/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

/*
    Scenetreemodel.cpp

    Provides a simple tree model to show how to create and use hierarchical
    models.
*/

#include "SceneTreeModel.h"
#include <QStringList>
#include "SceneObject.h"

//Scene Addon

TreeItem::TreeItem(std::shared_ptr<SceneObject> _object, TreeItem *_parent)
   : sceneObject(_object),
    parentItem(_parent)
{
    if(_parent)
        SceneObject::setParentChild(_parent->sceneObject, _object);

    itemData.insert(0,QVariant(QString(_object->getName().c_str())));
}

bool TreeItem::insertChildObject(int position, std::shared_ptr<SceneObject> _object)
{
    if (position < 0 || position > childItems.size())
        return false;

    TreeItem *item = new TreeItem(_object, this);
    childItems.insert(position, item);

    return true;
}

bool TreeItem::removeChildObject(int position)
{
    if (position < 0 || position > childItems.size())
        return false;
    delete childItems.takeAt(position);
    return true;
}

bool TreeItem::setStringData(std::string &_name)
{
    itemData.insert(0, QVariant(QString(_name.c_str())));
    sceneObject->setName(_name);
    return true;
}

bool SceneTreeModel::insertSceneObject(int position, std::shared_ptr<SceneObject> object, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    if (!parentItem)
        return false;

    beginInsertRows(parent, position, position);

    const bool success = parentItem->insertChildObject(position, object);
    endInsertRows();

    return success;
}

bool SceneTreeModel::removeSceneObject(const QModelIndex &_index)
{
    TreeItem *object = getItem(_index);
    if (!object)
        return false;

    auto parent_index = parent(_index);
    auto parentItem = object->parent();
    int pos = object->childNumber();

    beginRemoveRows(parent_index, pos, pos);
    parentItem->sceneObject->removeChild(pos);
    const bool success = parentItem->removeChildObject(pos);
    endRemoveRows();

    return success;
}

//QT

//! [0]
TreeItem::TreeItem(const QVector<QVariant> &data, TreeItem *parent)
    : itemData(data),
      parentItem(parent)
{}
//! [0]

//! [1]
TreeItem::~TreeItem()
{
    std::cout << sceneObject->getName() << " instances: " << sceneObject.use_count() << std::endl;
    qDeleteAll(childItems);
}
//! [1]

//! [2]
TreeItem *TreeItem::child(int number)
{
    if (number < 0 || number >= childItems.size())
        return nullptr;
    return childItems.at(number);
}
//! [2]

//! [3]
int TreeItem::childCount() const
{
    return childItems.count();
}
//! [3]

//! [4]
int TreeItem::childNumber() const
{
    if (parentItem)
        return parentItem->childItems.indexOf(const_cast<TreeItem*>(this));
    return 0;
}
//! [4]

//! [5]
int TreeItem::columnCount() const
{
    return itemData.count();
}
//! [5]

//! [6]
QVariant TreeItem::data(int column) const
{
    if (column < 0 || column >= itemData.size())
        return QVariant();
    return itemData.at(column);
}
//! [6]

//! [7]
bool TreeItem::insertChildren(int position, int count, int columns)
{
    if (position < 0 || position > childItems.size())
        return false;

    for (int row = 0; row < count; ++row) {
        QVector<QVariant> data(columns);
        TreeItem *item = new TreeItem(data, this);
        childItems.insert(position, item);
    }

    return true;
}
//! [7]

//! [8]
bool TreeItem::insertColumns(int position, int columns)
{
    if (position < 0 || position > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.insert(position, QVariant());

    for (TreeItem *child : qAsConst(childItems))
        child->insertColumns(position, columns);

    return true;
}
//! [8]

//! [9]
TreeItem *TreeItem::parent()
{
    return parentItem;
}
//! [9]

//! [10]
bool TreeItem::removeChildren(int position, int count)
{
    if (position < 0 || position + count > childItems.size())
        return false;

    for (int row = 0; row < count; ++row)
        delete childItems.takeAt(position);

    return true;
}
//! [10]

bool TreeItem::removeColumns(int position, int columns)
{
    if (position < 0 || position + columns > itemData.size())
        return false;

    for (int column = 0; column < columns; ++column)
        itemData.remove(position);

    for (TreeItem *child : qAsConst(childItems))
        child->removeColumns(position, columns);

    return true;
}

//! [11]
bool TreeItem::setData(int column, const QVariant &value)
{
    if (column < 0 || column >= itemData.size())
        return false;

    itemData[column] = value;
    sceneObject->setName(value.toString().toStdString());
    return true;
}
//! [11]

//! [8]
SceneTreeModel::SceneTreeModel(const std::string &_rootName, QObject *parent)
    : QAbstractItemModel(parent)
{
    auto rootObj = std::make_shared<SceneObject>();
    rootObj->setName(_rootName);
    m_rootItem = new TreeItem(rootObj);
}
//! [0]

//! [1]
SceneTreeModel::~SceneTreeModel()
{
    delete m_rootItem;
}
//! [1]

//! [2]
int SceneTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_rootItem->columnCount();
}
//! [2]

QVariant SceneTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    TreeItem *item = getItem(index);

    return item->data(index.column());
}

//! [3]
Qt::ItemFlags SceneTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}
//! [3]

//! [4]
TreeItem *SceneTreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        TreeItem *item = static_cast<TreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return m_rootItem;
}
//! [4]

QVariant SceneTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return m_rootItem->data(section);

    return QVariant();
}

//! [5]
QModelIndex SceneTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();
//! [5]

//! [6]
    TreeItem *parentItem = getItem(parent);
    if (!parentItem)
        return QModelIndex();

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}
//! [6]

bool SceneTreeModel::insertColumns(int position, int columns, const QModelIndex &parent)
{
    beginInsertColumns(parent, position, position + columns - 1);
    const bool success = m_rootItem->insertColumns(position, columns);
    endInsertColumns();

    return success;
}

bool SceneTreeModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    if (!parentItem)
        return false;

    beginInsertRows(parent, position, position + rows - 1);
    const bool success = parentItem->insertChildren(position,
                                                    rows,
                                                    m_rootItem->columnCount());
    endInsertRows();

    return success;
}

//! [7]
QModelIndex SceneTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = getItem(index);
    TreeItem *parentItem = childItem ? childItem->parent() : nullptr;

    if (parentItem == m_rootItem || !parentItem)
        return QModelIndex();

    return createIndex(parentItem->childNumber(), 0, parentItem);
}
//! [7]

bool SceneTreeModel::removeColumns(int position, int columns, const QModelIndex &parent)
{
    beginRemoveColumns(parent, position, position + columns - 1);
    const bool success = m_rootItem->removeColumns(position, columns);
    endRemoveColumns();

    if (m_rootItem->columnCount() == 0)
        removeRows(0, rowCount());

    return success;
}

bool SceneTreeModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    TreeItem *parentItem = getItem(parent);
    if (!parentItem)
        return false;

    beginRemoveRows(parent, position, position + rows - 1);
    const bool success = parentItem->removeChildren(position, rows);
    endRemoveRows();

    return success;
}

//! [8]
int SceneTreeModel::rowCount(const QModelIndex &parent) const
{
    const TreeItem *parentItem = getItem(parent);

    return parentItem ? parentItem->childCount() : 0;
}
//! [8]

bool SceneTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role != Qt::EditRole)
        return false;

    TreeItem *item = getItem(index);
    bool result = item->setData(index.column(), value);

    if (result)
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

    return result;
}

bool SceneTreeModel::setHeaderData(int section, Qt::Orientation orientation,
                              const QVariant &value, int role)
{
    if (role != Qt::EditRole || orientation != Qt::Horizontal)
        return false;

    const bool result = m_rootItem->setData(section, value);

    if (result)
        emit headerDataChanged(orientation, section, section);

    return result;
}