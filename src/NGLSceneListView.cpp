#include "NGLSceneListView.h"
#include <QGridLayout>
#include <QStringListModel>
#include <iostream>

NGLSceneListView::NGLSceneListView(QWidget *_parent)
{
    //find object list placeholder in the UI and replace it with our class
    auto layout = _parent->findChild<QGridLayout*>("s_listGridLayout");
    auto list_widget = _parent->findChild<QWidget*>("m_objList");
    layout->replaceWidget(list_widget, this);
    
    QStringList list;
    //list << "a" << "b";

    m_listModel = new QStringListModel(list);

    this->setModel(m_listModel);
}

void NGLSceneListView::push_back(const std::string &_name)
{
    auto numRows = m_listModel->rowCount();
    //Adding a new entry to a list tryQStringListModel
    //https://stackoverflow.com/questions/37412712/how-to-update-a-qstringlistmodel
    if(m_listModel->insertRow(numRows))
    {
        auto index = m_listModel->index(numRows,0);

        m_listModel->setData(index, QString(_name.c_str()));
    }
}