#include "SceneManager.h"
#include "ngl/VAOPrimitives.h"
#include "ngl/ShaderLib.h"
#include "Utils.h"
#include <QGridLayout>

#include "NGLObjectMenu.h"
#include "SceneLight.h"

#include <QFileDialog>
#include <QItemSelectionModel>

NGLScene * SceneManager::m_scene;
NGLSceneTreeView * SceneManager::m_list;
NGLObjectMenu * SceneManager::m_menu;

int SceneManager::new_id;
bool SceneManager::canDraw = true;
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

bool SceneManager::draw(const std::string &_shaderName)
{
    ngl::ShaderLib::use(_shaderName);
    if(canDraw) m_root->drawInherited(_shaderName);
    return true;
}

GLuint SceneManager::getDefaultTexture(int _i)
{
    switch(_i)
    {
        case 0:
            return m_scene->m_DefaultAlbedo;
        case 1:
            return m_scene->m_DefaultAORoughMet;
        case 2:
            return m_scene->m_DefaultNormal;
        case 3:
            return m_scene->m_DefaultEmissive;
    }
    return m_scene->m_DefaultAlbedo;
}

std::shared_ptr<SceneObject> SceneManager::addObject(const std::string &_name, SceneObject::ObjectType _type, const std::string &_path)
{   
    static int light_unique_id = 0;
    std::shared_ptr<SceneObject> obj;
    switch(_type)
    {
        case SceneObject::ObjectType::MESH:
        {
            std::cout << "\ntrying to create mesh";
            auto mesh = std::make_shared<SceneMesh>(_name,_path);

            mesh->m_material.textures[0] = m_scene->m_DefaultAlbedo;
            mesh->m_material.textures[1] = m_scene->m_DefaultAORoughMet;
            mesh->m_material.textures[2] = m_scene->m_DefaultNormal;

            obj = std::move(mesh);
            update();
            break;
        }
        case SceneObject::ObjectType::PRIMITIVE:
        {
            std::cout << "\nDetected primitive " << _path;
            auto prim = std::make_shared<SceneMesh>(_path);

            prim->m_material.textures[0] = m_scene->m_DefaultAlbedo;
            prim->m_material.textures[1] = m_scene->m_DefaultAORoughMet;
            prim->m_material.textures[2] = m_scene->m_DefaultNormal;

            obj = std::move(prim);
            
            break;
        }
        case SceneObject::ObjectType::LIGHT:
        {
            auto light = std::make_shared<SceneLight>(light_unique_id);
            m_scene->m_lights[light_unique_id] = light;
            
            m_scene->updateNumLights();

            std::cout << "\nLights: " << m_scene->m_lights.size();
            obj = std::move(light);
            obj->setName(_name+std::to_string(light_unique_id));
            light_unique_id++;
            break;
        }
    }

    m_list->add(obj);  
    update();
    return obj;
    
}

std::shared_ptr<SceneObject> SceneManager::addLight(ngl::Vec3 _pos, int _intensity, ngl::Vec3 _col,const std::string &_name)
{
    auto obj = addObject("light",SceneObject::ObjectType::LIGHT);
    
    auto light = std::static_pointer_cast<SceneLight>(obj);

    light->transform.setPosition(_pos);
    light->setIntensity(_intensity);
    light->setColour(_col);

    return light;
}


bool SceneManager::removeSelectedObject()
{
    if(m_selected)
    {
        if(m_selected->getType()==SceneObject::ObjectType::LIGHT)
        {
            m_scene->m_lights.erase(std::static_pointer_cast<SceneLight>(m_selected)->getID());
            m_scene->updateNumLights();
            std::cout << "\nLights: " << m_scene->m_lights.size();
        }
        m_list->removeSelected();
        update();
    }
    //clearScene();
    return true;
}

void SceneManager::clearScene()
{
    QModelIndex root = m_list->rootIndex();
    auto idx = m_list->model()->index(0,0,m_list->rootIndex());
    m_list->selectionModel()->setCurrentIndex(idx,QItemSelectionModel::Select);

    while(m_selected)
    {
        if(m_selected->getType()==SceneObject::ObjectType::LIGHT)
        {
            m_scene->m_lights.erase(std::static_pointer_cast<SceneLight>(m_selected)->getID());
            m_scene->updateNumLights();
            std::cout << "\nLights: " << m_scene->m_lights.size();
        }
        m_list->removeSelected();
        update();
    }
}


void SceneManager::update()
{
    m_scene->update();
    m_list->update();
    
}

void SceneManager::loadCameraMatrixToCurrentShader()
{
    m_scene->loadMatricesToShader();
}

void SceneManager::updateSelection()
{
    if(m_selected) m_selected->isSelected=false;
    m_selected = m_list->getSelectedObject();

    if(m_selected && m_selected != m_root)
    {   //std::cout<< "\n\nname " << m_selected->getName();
        m_menu->setVisible(true);
        m_selected->isSelected=true;
        m_menu->updateObject(m_selected);
    }
    else
    {
        m_menu->setVisible(false);
    }
}
