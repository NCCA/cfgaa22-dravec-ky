#include "SceneObject.h"
#include <iostream>

SceneObject::SceneObject()
{
    std::cout << "Scene Object Created!\n";
    transform.setPosition(ngl::Vec3(0.0f));
}

void SceneObject::setParentChild(std::shared_ptr<SceneObject> _parent, std::shared_ptr<SceneObject> _child)
{
    if(_child)
        _child->setParent(_parent);
    if(_parent)
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
    return (it-m_children.begin());
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
    
    std::cout << "Instances before deleting from parent: "<<m_children[index].use_count() << std::endl;
    m_children.erase(m_children.begin() + index);
    
    return true;
}

SceneObject::~SceneObject()
{
    std::cout << "\nScene Object " << m_name << " destroyed!\n";
}

void SceneObject::drawInherited()
{
    //std::cout << "\nObject " << m_name << " is drawing its children;\n";
    for(auto child : m_children)
    {
        //std::cout << child->m_name << std::endl;
        child->draw();
        child->drawInherited();
    }
}