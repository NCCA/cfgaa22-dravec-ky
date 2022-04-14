#include "SceneObject.h"
#include <iostream>

SceneObject::SceneObject()
{
    std::cout << "Scene Object Created!\n";
    q_data.push_back("object");

}

void SceneObject::setParentChild(std::shared_ptr<SceneObject> _parent, std::shared_ptr<SceneObject> _child)
{
    std::cout << "\nSetting parent...";
    _child->setParent(_parent);
    std::cout << "\nSetting child...";
    _parent->addChild(_child);
}

std::shared_ptr<SceneObject> SceneObject::getChild(int index)
{
    if (index < 0 || index >= m_children.size())
        return std::shared_ptr<SceneObject>();;
    return m_children.at(index);
}

int SceneObject::childCount() const
{
    return m_children.size();
}

int SceneObject::indexOf(std::shared_ptr<SceneObject> _child) const
{
    auto it = std::find(m_children.begin(), m_children.end(), std::move(_child));
    if (it == m_children.end())
    {
        return -1;
    }
}

bool SceneObject::setData(const QVariant &value)
{
    if(q_data.empty())
        q_data.push_back(value);
    else q_data[0] = value;

    return true;
}

QVariant SceneObject::data() const
{
    return q_data[0];
}

void SceneObject::setParent(std::shared_ptr<SceneObject> _parent)
{
    m_parent = _parent;
}
void SceneObject::addChild(std::shared_ptr<SceneObject> _child)
{
    m_children.push_back(_child);
}
bool SceneObject::removeChild(int index)
{
    m_children.erase(m_children.begin() + index);
    return true;
}

SceneObject::~SceneObject()
{
    std::cout << "\nScene Object Destroyed!\n";
}

void SceneObject::drawInherited()
{
    draw();
    for(auto child : m_children)
    {
        child->drawInherited();
    }
}