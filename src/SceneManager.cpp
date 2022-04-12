#include "SceneManager.h"


bool SceneManager::addObject(std::string &_name, ObjectType _type)
{   
    if(_type == ObjectType::MESH)
    {
        auto obj = std::make_unique<SceneMesh>();
        obj->load(_name);
    }
    return true;
}