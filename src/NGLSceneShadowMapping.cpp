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


void NGLScene::initDirShadowMaps()
{
    //Create shadow depth frame buffer
    glGenFramebuffers(1, &m_depthMapFBO);
    glGenTextures(1, &m_depthMap);
    glBindTexture(GL_TEXTURE_2D, m_depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                m_params.SHADOW_WIDTH, m_params.SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);  

    glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);  
}

void NGLScene::updateDirShadowSize()
{
  glBindTexture(GL_TEXTURE_2D, m_depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                m_params.SHADOW_WIDTH, m_params.SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

void NGLScene::initOmniShadowMaps()
{
  glGenFramebuffers(1, &m_depthMapFBO);
  glGenTextures(1, &m_depthCubeMap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMap);

  for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 
                     m_params.SHADOW_WIDTH, m_params.SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
  float borderColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

  glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthCubeMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);  

}

void NGLScene::updateOmniShadowSize()
{
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMap);
  for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 
                     m_params.SHADOW_WIDTH, m_params.SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}


void NGLScene::loadDirShadowMatrices()
{
  float near_plane = 1.0f, far_plane = 7.5f;
  ngl::Mat4 lightProjection = ngl::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
  ngl::Mat4 lightView = ngl::lookAt(ngl::Vec3(-2.0f, 4.0f, -1.0f), 
                                  ngl::Vec3( 0.0f, 0.0f,  0.0f), 
                                  ngl::Vec3( 0.0f, 1.0f,  0.0f));  
  ngl::Mat4 lightSpaceMatrix = lightProjection * lightView; 

  auto t = getViewProjection();

  ngl::ShaderLib::setUniformMatrix4fv("lightSpaceMatrix",&lightSpaceMatrix.m_00);
}

void NGLScene::loadOmniShadowMatrices()
{
  ngl::ShaderLib::use("shadowOmniMap");

  ngl::Vec3 lightPos = m_lightInfoArray[0].pos;

  float aspect = (float)m_params.SHADOW_WIDTH / (float)m_params.SHADOW_HEIGHT;
  float near_plane = 1.0f;
  float far_plane = 25.0f;
  ngl::Mat4 shadowProj = ngl::perspective(90.0f, aspect, near_plane, far_plane); 

  //6 view directions, each taking one face of the cubemap
  std::vector<ngl::Mat4> shadowTransforms;
  ngl::Vec3 center[] = {ngl::Vec3(1.0,0.0,0.0), ngl::Vec3(-1.0,0.0,0.0),
                        ngl::Vec3(0.0,1.0,0.0), ngl::Vec3(0.0,-1.0,0.0),
                        ngl::Vec3(0.0,0.0,1.0), ngl::Vec3(0.0,0.0,-1.0)};

  ngl::Vec3 up[] =     {ngl::Vec3(0.0,-1.0,0.0), ngl::Vec3(0.0,-1.0,0.0),
                        ngl::Vec3(0.0,0.0,1.0), ngl::Vec3(0.0,0.0,-1.0),
                        ngl::Vec3(0.0,-1.0,0.0), ngl::Vec3(0.0,-1.0,0.0)};

  for(int i=0;i<6;i++)
  { 
      auto lookAt = ngl::lookAt(lightPos, lightPos + center[i],up[i]);
      shadowTransforms.push_back(shadowProj * lookAt);
      //Utils::printMatrix(lookAt);
  }
  for(int i=0;i<6;++i)
     ngl::ShaderLib::setUniformMatrix4fv(fmt::format("shadowMatrices[{0}]",i),&shadowTransforms[i].m_00);

  ngl::ShaderLib::setUniform("lightPos",lightPos);
  ngl::ShaderLib::setUniform("far_plane",far_plane);
  ngl::ShaderLib::use("PBR");
  ngl::ShaderLib::setUniform("lightPos",lightPos);
  ngl::ShaderLib::setUniform("far_plane", far_plane);
  ngl::ShaderLib::use("DeferredPBR");
  ngl::ShaderLib::setUniform("lightPos",lightPos);
  ngl::ShaderLib::setUniform("far_plane", far_plane);

}

void NGLScene::renderDirShadowMaps()
{
    GLint prev_fbo;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbo);

    glViewport(0, 0, m_params.SHADOW_WIDTH, m_params.SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
      glClear(GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);
      ngl::ShaderLib::use("shadowMap");
      loadDirShadowMatrices();
      SceneManager::draw("shadowMap");

    glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);
}

void NGLScene::renderOmniShadowMaps()
{
    
    GLint prev_fbo;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbo);
    
    glViewport(0, 0, m_params.SHADOW_WIDTH, m_params.SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
      glClear(GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);
      glCullFace(GL_FRONT);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMap);
      loadOmniShadowMatrices();
      SceneManager::draw("shadowOmniMap");

    glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);
    glCullFace(GL_BACK);
}