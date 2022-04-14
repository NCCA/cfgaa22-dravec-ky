#ifndef SCENE_MANAGER_H_
#define SCENE_MANAGER_H_

#include <ngl/Transformation.h>
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include <ngl/Text.h>
#include <iostream>
#include <QListView>
#include <QStringListModel>

#include "NGLScene.h"
#include "NGLSceneTreeView.h"
#include "SceneObject.h"
#include "SceneMesh.h"

//Temporary wrapper for calling VAOPrimitives draw functions using the Scene Manager for testing
class ScenePrimitive : public SceneObject
{
  public:
    ScenePrimitive(const std::string &_name);
    void draw () override;
    ~ScenePrimitive() {};
};

class SceneManager
{
  public:
    static bool initialize(NGLScene * _scene, NGLSceneTreeView * _list);

    enum class ObjectType {MESH, LIGHT, PRIMITIVE};
    static bool addObject(const std::string &_name = "", ObjectType _type = ObjectType::MESH, const std::string &_path = NULL);
    static bool removeObject(int index);
    static bool draw();
    static void loadObject();

  private:
    static std::vector<std::shared_ptr<SceneObject>> m_objects;
    static std::shared_ptr<SceneObject> parent;
    static const std::array<std::string, 3> m_primitives;
    static NGLScene * m_scene;
    static NGLSceneTreeView * m_list;
    static int new_id;
    static std::shared_ptr<SceneObject> m_root;
};

#endif