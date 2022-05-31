#include "NGLScene.h"
#include "SceneManager.h"
#include "Utils.h"
#include <iostream>
#include <cstddef>
#include <ngl/Vec3.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <ngl/Obj.h>
#include <ngl/Util.h>
#include <QColorDialog>
#include <QFileDialog>
#include <GLFW/glfw3.h>
#include <QTimer>
#include <ngl/Random.h>
#include <QProgressDialog>
#include <QProgressBar>

void NGLScene::initDefaultAssets()
{
//Initialize default empty textures for materials
  glGenTextures(1, &m_DefaultAlbedo);
  glBindTexture(GL_TEXTURE_2D, m_DefaultAlbedo);
  GLubyte texData0[] = { 255, 255, 255, 255 };
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData0);

  glGenTextures(1, &m_DefaultAORoughMet);
  glBindTexture(GL_TEXTURE_2D, m_DefaultAORoughMet);
  GLubyte texData1[] = { 25, 150, 150, 255 };
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData1);

  glGenTextures(1, &m_DefaultNormal);
  glBindTexture(GL_TEXTURE_2D, m_DefaultNormal);
  GLubyte texData2[] = { 0, 0, 255, 255 };
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData2);
  
  glGenTextures(1, &m_DefaultEmissive);
  glBindTexture(GL_TEXTURE_2D, m_DefaultEmissive);
  GLubyte texData3[] = { 0, 0, 0, 255 };
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData3);
  

  ngl::VAOPrimitives::createTrianglePlane("plane",1,1,1,1,ngl::Vec3(0,1,0));
  ngl::VAOPrimitives::createTorus("torus",0.3,1,8,16);
  ngl::VAOPrimitives::createCylinder("cylinder",0.8,2,16,3);
  ngl::VAOPrimitives::createSphere("sphere",1,64);
  ngl::VAOPrimitives::createSphere("lightSphere",1,8);
  createScreenQuad();

  auto tex = ngl::Texture("../textures/rustediron2_basecolor.png");
  m_test_tex_id = tex.setTextureGL();
}

void NGLScene::loadScene1()
{
  SceneManager::clearScene();
  m_v_rot.identity();
    m_v_trans.identity();
    m_v_scale.identity();

  ngl::Vec3 eye(0.0f,3.0f,3.0f);
  ngl::Vec3 look(0,1,0);
  ngl::Vec3 up(0,1,0);
  m_view = ngl::lookAt(eye,look,up);
  SceneManager::addLight(ngl::Vec3(0,3,0),10, ngl::Vec3(0.0f,0.0f,1.0f));
  SceneManager::addLight(ngl::Vec3(-3,3,0),10, ngl::Vec3(1.0f,0.0f,0.0f));
  SceneManager::addLight(ngl::Vec3(0.0,10.5,8.1),100, ngl::Vec3(1.0f,1.0f,0.5f));

  //auto obj = SceneManager::addObject("plane", SceneObject::ObjectType::PRIMITIVE, "plane");
  //obj->transform.setPosition(0.0f,-0.5f,0.0f);
  //obj->transform.setScale(10.0,10.0f,10.0f);
  auto obj = SceneManager::addObject("teapot", SceneObject::ObjectType::PRIMITIVE, "teapot");
  obj->transform.setPosition(0,1,0);
  obj.reset();
  obj = SceneManager::addObject("hall", SceneObject::ObjectType::MESH, "models/hall.obj");
  obj->loadTexture(0,"textures/hall_Albedo.png");
  obj->loadTexture(1,"textures/hall_AORoughMet.png");
  obj->loadTexture(2,"textures/hall_Normal.png");
  obj->loadTexture(3,"textures/hall_Emissive.png");
  obj->transform.setScale(20,20,20);
  obj->transform.setPosition(0,0,80);

  updateNumLights();
}

void NGLScene::createScreenQuad()
{
  std::vector<GLfloat> quadVertices = {
        // positions        // texture Coords
        -1.0f,  -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,

        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f,  -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
  std::unique_ptr<ngl::AbstractVAO> m_vao = ngl::SimpleVAO::create(GL_TRIANGLES);
  auto data = ngl::AbstractVAO::VertexData(quadVertices.size()*sizeof(GLfloat),quadVertices[0]);
  m_vao->bind();
  m_vao->setData(data);
  m_vao->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(GLfloat)*5,0);
  m_vao->setVertexAttributePointer(1,2,GL_FLOAT,sizeof(GLfloat)*5,3);
  m_vao->setNumIndices(6);
  m_vao->unbind();
  std::cout << "\n\n\n\n simple vao quad: " << m_vao->numIndices();
  ngl::VAOPrimitives::addToPrimitives("screenQuad",std::move(m_vao));
}

void NGLScene::loadSceneCornell()
{
  SceneManager::clearScene();
  m_v_rot.identity();
  m_v_trans.identity();
  m_v_scale.identity();

  ngl::Vec3 eye(2.7f,2.7f,12.0f);
  ngl::Vec3 look(2.7,2.7,2.4);
  ngl::Vec3 up(0,1,0);
  m_view = ngl::lookAt(eye,look,up);
  
  SceneManager::addLight(ngl::Vec3(1,3,3),3, ngl::Vec3(1.0f,0.3f,0.3f));
  SceneManager::addLight(ngl::Vec3(4.3,3,3.3),3, ngl::Vec3(0.3f,1.0f,0.3f));
  SceneManager::addLight(ngl::Vec3(2.7,4.3,3.7),9, ngl::Vec3(1.0f,1.0f,0.6f));

  auto obj =       SceneManager::addObject("wallmid", 
                  SceneObject::ObjectType::MESH, 
                  "models/cornellbox/wallmid.obj");
  obj->transform.setScale(0.03,0.03,0.03);
  Material mat;
  mat.roughness=0.7;
  mat.metallic=0;
  obj->setMaterialInfo(mat);

  obj =  SceneManager::addObject("wallright", 
                  SceneObject::ObjectType::MESH, 
                  "models/cornellbox/wallright.obj");
  obj->transform.setScale(0.03,0.03,0.03);
  mat.albedo = ngl::Vec3(0,1,0);
  obj->setMaterialInfo(mat);

  obj =       SceneManager::addObject("wallleft", 
                  SceneObject::ObjectType::MESH, 
                  "models/cornellbox/wallleft.obj");
  mat.albedo = ngl::Vec3(1,0,0);
  obj->setMaterialInfo(mat);

  obj->transform.setScale(0.03,0.03,0.03);
  obj =       SceneManager::addObject("boxleft", 
                  SceneObject::ObjectType::MESH, 
                  "models/cornellbox/boxleft.obj");
  mat.albedo = ngl::Vec3(0.25,0.5,1);
  obj->setMaterialInfo(mat);

  obj->transform.setScale(0.03,0.03,0.03);
  obj =       SceneManager::addObject("sphere1", 
                  SceneObject::ObjectType::PRIMITIVE,
                  "sphere");

  mat.albedo = ngl::Vec3(1,1,1);
  mat.metallic = 1;
  mat.roughness = 0.6;
  obj->setMaterialInfo(mat);
  obj->transform.setScale(1,1,1);
  obj->transform.setPosition(3.7,0.9,3.4);

  updateNumLights();
}

void NGLScene::loadSceneSponza()
{
  QProgressDialog d;
  d.setBar(new QProgressBar());
  d.setLabel(new QLabel(QString("Loading scene...")));
  d.setMaximum(50);
  d.setValue(10);
  d.show();

  SceneManager::clearScene();
  m_v_rot.identity();
  m_v_trans.identity();
  m_v_scale.identity();

  ngl::Vec3 eye(-11.44f,1.01f,-0.4f);
  ngl::Vec3 look(-1.16,0.8,0.0);
  ngl::Vec3 up(0,1,0);
  m_view = ngl::lookAt(eye,look,up);

  SceneManager::addLight(ngl::Vec3(12.6,3.9,-0.4),80, ngl::Vec3(1.0f,0.3f,0.0f));
  SceneManager::addLight(ngl::Vec3(1.3,6.4,-0.2),50, ngl::Vec3(0.0f,0.25f,0.5f));
  //SceneManager::addLight(ngl::Vec3(-0.6,1.8,4.2),10, ngl::Vec3(1.0f,0.3f,0.0f));
  //SceneManager::addLight(ngl::Vec3(-0.5,1.9,-4.7),20, ngl::Vec3(1.0f,0.3f,0.0f));

  auto obj =       SceneManager::addObject("sponza", 
                  SceneObject::ObjectType::MESH, 
                  "models/sponza/sponza_uvd.obj");
  obj->transform.setScale(0.01,0.01,0.01);
  obj->loadTexture(0,"textures/sponza_Albedo.jpg");
  obj->loadTexture(1,"textures/sponza_ORM.jpg");
  obj->loadTexture(2,"textures/sponza_Normal.jpg");
  Material mat;
  mat.roughness=0.6;
  mat.metallic=0.7;
  mat.emissive=0.0;
  mat.ao=0.0;
  mat.normal=0.3;
  obj->setMaterialInfo(mat);

  updateNumLights();
  d.setValue(50);
}

void NGLScene::loadScenePBR()
{
  SceneManager::clearScene();
  m_v_rot.identity();
  m_v_trans.identity();
  m_v_scale.identity();

  ngl::Vec3 eye(0.0f,20.0f,16.0f);
  ngl::Vec3 look(0,0,1);
  ngl::Vec3 up(0,1,0);
  m_view = ngl::lookAt(eye,look,up);

  SceneManager::addLight(ngl::Vec3(-8,10.0,8),20, ngl::Vec3(1.0f,1.0f,1.0f));
  SceneManager::addLight(ngl::Vec3(-8,10.0,-8),20, ngl::Vec3(1.0f,1.0f,1.0f));
  SceneManager::addLight(ngl::Vec3(8,10.0,8),20, ngl::Vec3(1.0f,1.0f,1.0f));
  SceneManager::addLight(ngl::Vec3(8,10.0,-8),20, ngl::Vec3(1.0f,1.0f,1.0f));

  SceneManager::addLight(ngl::Vec3(0.0,10.0,0.0),50, ngl::Vec3(1.0f,1.0f,1.0f));

  auto obj =  SceneManager::addObject("floor", 
                  SceneObject::ObjectType::PRIMITIVE, 
                  "plane");
  obj->transform.setScale(50,50,50);
  Material mat;
  mat.roughness=1.0;
  mat.metallic=0;
  mat.albedo=ngl::Vec3(0.0);
  obj->setMaterialInfo(mat);
  mat.albedo=ngl::Vec3(0.7);

  float offset = 8;
  float step = offset*2/5;
  int i=0;

  for(float x=-offset; x<=offset; x+=step)
  {
    for(float z=-offset; z<=offset; z+=step)
    {
      obj = SceneManager::addObject(fmt::format("sphere{}",i), 
                  SceneObject::ObjectType::PRIMITIVE, 
                  "sphere");
      obj->transform.setPosition(ngl::Vec3(x,1,z));
      mat.roughness=(x+offset)/(2*offset)+0.001;
      mat.metallic=0.7*(z+offset)/(2*offset);
      obj->setMaterialInfo(mat);
      i++;
    }
  }
  updateNumLights();
}



void NGLScene::loadSceneLights()
{
  SceneManager::clearScene();
  m_v_rot.identity();
  m_v_trans.identity();
  m_v_scale.identity();

  ngl::Vec3 eye(2.7f,2.7f,12.0f);
  ngl::Vec3 look(2.7,2.7,2.4);
  ngl::Vec3 up(0,1,0);
  m_view = ngl::lookAt(eye,look,up);

  Material mat;
  int i=0;
  int num = 1000;
  float offset = 12;
  for(int i=0; i< num; ++i)
  {
        auto obj = SceneManager::addObject(fmt::format("sphere{}",i), 
                    SceneObject::ObjectType::PRIMITIVE, 
                    "teapot");
        obj->transform.setPosition(ngl::Random::getRandomVec3()*offset);
        obj->transform.setRotation(ngl::Random::getRandomVec3()*180);
        mat.roughness=0.7;
        mat.metallic=0.7;
        mat.albedo = ngl::Vec3(ngl::Random::randomPositiveNumber());
        obj->setMaterialInfo(mat);
  }

  num = 300;
  for(int i=0; i< num; ++i)
  {
      auto pos = ngl::Random::getRandomVec3()*offset;
      auto col = ngl::Random::getRandomColour3();
      int intensity = 1;
      SceneManager::addLight(pos, intensity, col, std::to_string(i));
  }

  updateNumLights();
}