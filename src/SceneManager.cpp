#include "SceneManager.h"
#include "ngl/VAOPrimitives.h"
#include <QGridLayout>

std::vector<std::shared_ptr<SceneObject>> SceneManager::m_objects;

const std::array<std::string, 3> SceneManager::m_primitives;
NGLScene * SceneManager::m_scene;
NGLSceneTreeView * SceneManager::m_list;
int SceneManager::new_id;
std::shared_ptr<SceneObject> SceneManager::m_root;

ScenePrimitive::ScenePrimitive(const std::string &_name)
{
    m_name = _name;
    //std::cout << m_name << " has been loaded!\n";
}

void ScenePrimitive::draw()
{
    ngl::VAOPrimitives::draw(m_name);
}

bool SceneManager::initialize(NGLScene * _scene, NGLSceneTreeView * _list)
{
    m_scene = _scene;
    m_list = _list;
    m_root = m_list->getSceneRoot();
    return true;

}

bool SceneManager::draw()
{
    m_root->drawInherited();
    return true;
}

bool SceneManager::addObject(const std::string &_name, ObjectType _type, const std::string &_path)
{   
    std::shared_ptr<SceneObject> obj;
    switch(_type)
    {
        case ObjectType::MESH:
        {
            auto mesh = std::make_shared<SceneMesh>();
            if(mesh->load(_name))
            {
            }
        }
        case ObjectType::PRIMITIVE:
        {
            std::cout << "\nDetected primitive " << _path;
            auto prim = std::make_shared<ScenePrimitive>(_path);
            obj = std::move(prim);
        }
    }

    m_list->add(obj);   
    update();

    ++new_id;
    return true;
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