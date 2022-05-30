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

void NGLScene::paintGL()
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_mainFBO);
    //glCullFace(GL_BACK);
    renderOmniShadowMaps();
    //glCullFace(GL_FRONT);

    renderDeferred();
    //renderForward();
    //renderTexture(m_gBufferAORoughnessMetallic)


    glBindFramebuffer(GL_FRAMEBUFFER, m_mainFBO);
}

void NGLScene::initDeferred()
{
  std::cout << "\n\n\nWindow size during deferred init: " <<  m_win.height << " " << m_win.width;
  //Note that we use GL_RGBA16F over GL_RGB16F as GPUs generally prefer 4-component 
  //formats over 3-component formats due to byte alignment; some drivers may fail 
  //to complete the framebuffer otherwise. 

  glGenFramebuffers(1, &m_gBuffer);
  glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

  auto size = this->size();
  int width = size.width();
  int height = size.height();

  //position color buffer
  // glGenTextures(1, &m_gBufferPosition);
  // glBindTexture(GL_TEXTURE_2D, m_gBufferPosition);
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_gBufferPosition, 0);

  // - normal color buffer
  glGenTextures(1, &m_gBufferNormal);
  glBindTexture(GL_TEXTURE_2D, m_gBufferNormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gBufferNormal, 0);
    
  // - albedo buffer
  glGenTextures(1, &m_gBufferAlbedo);
  glBindTexture(GL_TEXTURE_2D, m_gBufferAlbedo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gBufferAlbedo, 0);

  // - metallic roughness ao buffer
  glGenTextures(1, &m_gBufferAORoughnessMetallic);
  glBindTexture(GL_TEXTURE_2D, m_gBufferAORoughnessMetallic);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gBufferAORoughnessMetallic, 0);
  
  // - emissive
  glGenTextures(1, &m_gBufferEmissive);
  glBindTexture(GL_TEXTURE_2D, m_gBufferEmissive);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_gBufferEmissive, 0);
  
  //depth
  glGenTextures(1, &m_gBufferDepth);
    glBindTexture(GL_TEXTURE_2D, m_gBufferDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 
                width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_gBufferDepth, 0);

  // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
  unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
  glDrawBuffers(4, attachments);
  glEnable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  ngl::ShaderLib::use("DeferredPBR");
  loadLightsToShader();
}


void NGLScene::loadShaderDefaults()
{
  auto _name = ngl::ShaderLib::getCurrentShaderName();
  if(_name == "PBR" || _name == "gBufferPBR")
  {
    ngl::ShaderLib::setUniform("shadowCubeMap",0);
    ngl::ShaderLib::setUniform("tex_Albedo",1);
    ngl::ShaderLib::setUniform("tex_AORoughMet",2);
    ngl::ShaderLib::setUniform("tex_Normal",3);
    ngl::ShaderLib::setUniform("tex_Emissive",4);
    
  }
  else if(_name=="DeferredPBR")
  {
    ngl::ShaderLib::setUniform("shadowCubeMap",0);
    ngl::ShaderLib::setUniform("gAlbedo",1);
    ngl::ShaderLib::setUniform("gAORoughMet",2);
    ngl::ShaderLib::setUniform("gNormal",3);
    ngl::ShaderLib::setUniform("gEmissive",4);
    ngl::ShaderLib::setUniform("gDepth",5);
    
  }
}


void NGLScene::renderForward()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_mainFBO);
  glViewport(0,0,m_win.width,m_win.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    ngl::ShaderLib::use("PBR");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMap);
    SceneManager::draw("PBR");
}


void NGLScene::updateDeferredSize()
{
  auto size = this->size();
  int width = size.width();
  int height = size.height();

  // glBindTexture(GL_TEXTURE_2D, m_gBufferPosition);
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, m_gBufferNormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, m_gBufferAlbedo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, m_gBufferAORoughnessMetallic);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, m_gBufferEmissive);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, m_gBufferDepth);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

}

void NGLScene::renderDeferred()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
  glViewport(0,0,m_win.width,m_win.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SceneManager::draw("gBufferPBR");

  glBindFramebuffer(GL_FRAMEBUFFER, m_mainFBO);
    ngl::ShaderLib::use("DeferredPBR");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gBufferAlbedo);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gBufferAORoughnessMetallic);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_gBufferNormal);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_gBufferEmissive);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, m_gBufferDepth);

    transform t = getViewProjection();
    t.V = t.V.inverse();
    t.P = t.P.inverse();
    t.VP = t.VP.inverse();
    ngl::ShaderLib::setUniformBuffer("InverseVP",sizeof(transform),&t.V.m_00);

    ngl::VAOPrimitives::draw("screenQuad");
}

void NGLScene::renderTexture(const GLuint &tex_id)
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_mainFBO);
  glViewport(0,0,m_win.width,m_win.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ngl::ShaderLib::use("Screen");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_id);
  ngl::VAOPrimitives::draw("screenQuad");
}
