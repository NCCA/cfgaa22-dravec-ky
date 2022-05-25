#include "SceneManager.h"
#include "ngl/VAOPrimitives.h"
#include "ngl/ShaderLib.h"
#include "Utils.h"
#include <QGridLayout>
#include "NGLObjectMenu.h"
#include "ScenePrimitive.h"

#include <QFileDialog>

NGLScene * SceneManager::m_scene;
NGLSceneTreeView * SceneManager::m_list;
NGLObjectMenu * SceneManager::m_menu;

int SceneManager::new_id;
std::shared_ptr<SceneObject> SceneManager::m_root;
std::shared_ptr<SceneObject> SceneManager::m_selected;

bool SceneManager::initialize(NGLScene * _scene, NGLSceneTreeView * _list, NGLObjectMenu * _menu)
{
    m_scene = _scene;
    m_list = _list;
    m_menu = _menu;
    m_root = m_list->getSceneRoot();
    updateSelection();
    return true;
}

bool SceneManager::draw()
{
    m_root->drawInherited();
    return true;
}

std::shared_ptr<SceneObject> SceneManager::addObject(const std::string &_name, ObjectType _type, const std::string &_path)
{   

    std::shared_ptr<SceneObject> obj;
    switch(_type)
    {
        case ObjectType::MESH:
        {
            auto mesh = std::make_shared<SceneMesh>(_name,_path);
            obj = std::move(mesh);
            break;
        }
        case ObjectType::PRIMITIVE:
        {
            std::cout << "\nDetected primitive " << _path;
            auto prim = std::make_shared<SceneMesh>(_path);
            obj = std::move(prim);
            break;
        }
    }

    m_list->add(obj);   
    update();

    ++new_id;
    return obj;
    
}

bool SceneManager::removeSelectedObject()
{
    m_list->removeSelected();
    update();
    return true;
}

void SceneManager::update()
{
    m_scene->update();
    m_list->update();
    
}

void SceneManager::updateSelection()
{
    if(m_selected) m_selected->isSelected=false;
    m_selected = m_list->getSelectedObject();

    if(m_selected && m_selected != m_root)
    {   //std::cout<< "\n\nname " << m_selected->getName();
        m_selected->isSelected=true;
        m_menu->setVisible(true);
        m_menu->updateObject(m_selected);
    }
    else
    {
        m_menu->setVisible(false);
    }
}
