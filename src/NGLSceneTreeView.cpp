#include "NGLSceneTreeView.h"
#include <QGridLayout>
#include <QStringListModel>
#include <QModelIndexList>
#include <iostream>
#include <QVariant>
#include <QVector>
#include <QFile>
#include "SceneObject.h"
#include "SceneManager.h"

NGLSceneTreeView::NGLSceneTreeView(QWidget *_parent)
{
    //find object list placeholder in the UI and replace it with our class
    
    QStringList headers;
    QString data;
    //list << "a" << "b";

    headers = QStringList(QString("root"));
    
    m_treeModel = new SceneTreeModel("Object Name");

    this->setModel(m_treeModel);
    this->setSelectionMode(QAbstractItemView::SelectionMode::SingleSelection);
    
    
    isLoaded = true;
    // for (int column = 0; column < m_treeModel->columnCount(); ++column)
    //     this->resizeColumnToContents(column);
}

void NGLSceneTreeView::add(std::shared_ptr<SceneObject> obj)
{
    //std::cout<<"Pushing back..\n";
    auto numRows = m_treeModel->rowCount();

    //https://stackoverflow.com/questions/37412712/how-to-update-a-qstringlistmodel
    //std::cout<<"Finished checking row count...\n";
    if(m_treeModel->insertSceneObject(numRows,obj))
    {
        update();
    }
}


void NGLSceneTreeView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Delete)
    {
        SceneManager::removeSelectedObject();
    }
}

void NGLSceneTreeView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    this->update();
    if(isLoaded)
    {
        selectionChangedSignal();
        //std::cout << "selection chnaged";
        SceneManager::updateSelection();
    }
}


void NGLSceneTreeView::mousePressEvent(QMouseEvent *event)
{
    //https://stackoverflow.com/questions/2761284/is-it-possible-to-deselect-in-a-qtreeview-by-clicking-off-an-item
    //Deselects items when clicked away
    
    QModelIndex item = indexAt(event->pos());

    bool selected = selectionModel()->isSelected(indexAt(event->pos()));
    
    QTreeView::mousePressEvent(event);

    if ((item.row() == -1 && item.column() == -1) || selected)
    {
        clearSelection();
        const QModelIndex index;
        selectionModel()->setCurrentIndex(index, QItemSelectionModel::Select);
    }
}

std::shared_ptr<SceneObject> NGLSceneTreeView::getSceneRoot()
{
    return m_treeModel->getRootObject();
}

bool NGLSceneTreeView::removeSelected()
{
    auto model = this->selectionModel();
    auto indexes = model->selectedIndexes();
    if (indexes.size() > 0)
    {
        QModelIndex selectedIndex = indexes.at(0);
        bool success = m_treeModel->removeSceneObject(selectedIndex);
        return success;
    }
    return false;
}

bool NGLSceneTreeView::removeChildren(const QModelIndex &_idx)
{
    m_treeModel->removeSceneObjectChildren(_idx);
    return false;
}

std::shared_ptr<SceneObject> NGLSceneTreeView::getSelectedObject()
{
    auto model = this->selectionModel();
    auto indexes = model->selectedIndexes();
    if (indexes.size() > 0)
    {
        QModelIndex selectedIndex = indexes.at(0);
        return m_treeModel->getSceneObject(selectedIndex);
    }
    return nullptr;
}
