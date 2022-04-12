#include "SceneManager.h"
#include "ngl/VAOPrimitives.h"
#include <QGridLayout>

std::vector<std::unique_ptr<SceneObject>> SceneManager::m_objects;
const std::array<std::string, 3> SceneManager::m_primitives;
NGLScene * SceneManager::m_scene;
NGLSceneListView * SceneManager::m_list;

ScenePrimitive::ScenePrimitive(const std::string &_name)
{
    m_name = _name;
    std::cout << m_name << " has been loaded!\n";
}

void ScenePrimitive::draw()
{
    ngl::VAOPrimitives::draw(m_name);
}

bool SceneManager::initialize(NGLScene * _scene, NGLSceneListView * _list)
{
    m_scene = _scene;
    m_list = _list;
    return true;
    m_list->push_back("try");
}

bool SceneManager::draw()
{
    std::cout << ".";
    for(auto const & obj: m_objects)
    {
        std::cout << obj->getName() << std::endl;
        obj->draw();
    };
    return true;
}

bool SceneManager::addObject(const std::string &_name, ObjectType _type, const std::string &_path)
{   
    std::unique_ptr<SceneObject> obj;
    switch(_type)
    {
        case ObjectType::MESH:
        {
            auto mesh = std::make_unique<SceneMesh>();
            if(mesh->load(_name))
            {
            }
        }
        case ObjectType::PRIMITIVE:
        {
            std::cout << "Deteted primitive " << _path;

            auto prim = std::make_unique<ScenePrimitive>(_path);

            obj = std::move(prim);
        }
    }
    int curIndex = m_objects.size();
    obj->setIndex(curIndex);

    m_list->push_back(std::to_string(obj->getIndex()));
    m_objects.push_back(std::move(obj));
    
    m_scene->update();

    return true;
}