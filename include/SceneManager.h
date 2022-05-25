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
#include "NGLObjectMenu.h"
#include "MainWindow.h"


class SceneManager
{
  public:
    static bool initialize(NGLScene * _scene, NGLSceneTreeView * _list, NGLObjectMenu * _menu);

    enum class ObjectType {MESH, LIGHT, PRIMITIVE};
    static std::shared_ptr<SceneObject> addObject(const std::string &_name = "", ObjectType _type = ObjectType::MESH, const std::string &_path = NULL);
    static bool removeSelectedObject();
    static bool draw();
    static void loadObject();
    static void update();
    static void updateSelection();
    static std::shared_ptr<SceneObject> getSelectedObject() {return m_selected;};
    static std::shared_ptr<SceneObject> m_selected;

  private:
    static NGLScene * m_scene;
    static NGLSceneTreeView * m_list;
    static NGLObjectMenu * m_menu;


    static int new_id;
    static std::shared_ptr<SceneObject> m_root;

    
};

#endif