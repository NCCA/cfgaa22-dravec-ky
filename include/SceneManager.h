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
#include "SceneLight.h"

#include "NGLObjectMenu.h"
#include "MainWindow.h"


class SceneManager
{
  public:
    static bool initialize(NGLScene * _scene, NGLSceneTreeView * _list, NGLObjectMenu * _menu);

    
    static std::shared_ptr<SceneObject> addObject(const std::string &_name = "default", SceneObject::ObjectType _type = SceneObject::ObjectType::PRIMITIVE, const std::string &_path = "teapot");
    static std::shared_ptr<SceneObject> addLight(ngl::Vec3 _pos = ngl::Vec3(0.0f,0.0f,0.0f), int _intensity = 10, ngl::Vec3 _col = ngl::Vec3(1.0f,1.0f,1.0f),const std::string &_name = "light");

    static bool removeSelectedObject();
    static bool draw(const std::string &_shaderName = "PBR");
    static void loadObject();
    static void update();
    static void updateSelection();
    static std::shared_ptr<SceneObject> getSelectedObject() {return m_selected;};
    static std::shared_ptr<SceneObject> m_selected;
    static void loadCameraMatrixToCurrentShader();
    static void updateLightInfo() {m_scene->updateLightInfo();};
    static transform getViewProjection() {return m_scene->getViewProjection();};
    static GLuint getDefaultTexture(int _index);
    static void clearScene();
    static bool setRenderFunction(void (NGLScene::*func)() ) { return m_scene->setRenderFunction(func); };


  private:
    static NGLScene * m_scene;
    static NGLSceneTreeView * m_list;
    static NGLObjectMenu * m_menu;
    static std::string m_curShader;


    static int new_id;
    static std::shared_ptr<SceneObject> m_root;
    static bool canDraw;

    
    
};

#endif