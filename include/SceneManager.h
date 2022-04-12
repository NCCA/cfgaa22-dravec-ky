#ifndef SCENE_MANAGER_H_
#define SCENE_MANAGER_H_

#include <ngl/Transformation.h>
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include <ngl/Text.h>
#include <iostream>
#include "SceneObject.h"
#include "SceneMesh.h"

class SceneManager
{
  public:
    enum class ObjectType {MESH, LIGHT};
    SceneManager() {}
    ~SceneManager() {}
    bool addObject(std::string &_name, ObjectType _type = ObjectType::MESH);
    bool removeObject(int index);

  private:
    std::vector<std::unique_ptr<SceneObject>> m_objects;
    const std::array<std::string, 3> m_primitives = {"sphere", "teapot", "cube"};

};

#endif