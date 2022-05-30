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

void NGLScene::initDefaultAssets()
{
//Initialize default empty textures for materials
  glGenTextures(1, &m_DefaultAlbedo);
  glBindTexture(GL_TEXTURE_2D, m_DefaultAlbedo);
  GLubyte texData0[] = { 220, 220, 220, 255 };
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
  ngl::VAOPrimitives::createSphere("sphere",1,32);
  ngl::VAOPrimitives::createSphere("lightSphere",1,8);
  createScreenQuad();

  auto tex = ngl::Texture("../textures/rustediron2_basecolor.png");
  m_test_tex_id = tex.setTextureGL();
}

void NGLScene::loadScene1()
{
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