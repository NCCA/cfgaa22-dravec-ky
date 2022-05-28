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


NGLScene::NGLScene( QWidget *_parent ) : QOpenGLWidget( _parent )
{
  setFocus();
  setFocusPolicy(Qt::StrongFocus);
  this->resize(_parent->size());
	m_wireframe=false;
	m_rotation=0.0;
	m_scale=1.0;
}

void NGLScene::initializeGL()
{
  
  ngl::NGLInit::initialize();
  //prepare some basic shapes
  ngl::VAOPrimitives::createTrianglePlane("plane",1,1,1,1,ngl::Vec3(0,1,0));
  ngl::VAOPrimitives::createTorus("torus",0.3,1,8,16);
  ngl::VAOPrimitives::createCylinder("cylinder",0.8,2,16,3);
  ngl::VAOPrimitives::createSphere("sphere",1,16);
  createScreenQuad();

  //std::shared_ptr<SceneObject> obj;
  // for(int i=0;i<10;i++)
  // {
  //   obj = SceneManager::addObject("", SceneManager::ObjectType::PRIMITIVE, "teapot");
  //   obj->transform.setPosition(ngl::Vec3(i));
  //   SceneManager::update();
  // }

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);			   // Grey Background
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_LINE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

  ngl::Vec3 eye(0.0f,2.0f,2.0f);
  ngl::Vec3 look(0,0,0);
  ngl::Vec3 up(0,1,0);
  m_view = ngl::lookAt(eye,look,up);

  m_project=ngl::perspective(45,float(1024/720),0.1f,300.0f);

  // in the code create some constexpr
  
  // create the shader program

  m_omniLight = SceneManager::addLight(ngl::Vec3(3,3,0),10, ngl::Vec3(1.0f,1.0f,1.0f));
  SceneManager::addLight(ngl::Vec3(0,3,0),10, ngl::Vec3(0.0f,0.0f,1.0f));
  SceneManager::addLight(ngl::Vec3(-3,3,0),10, ngl::Vec3(1.0f,0.0f,0.0f));
  auto obj = SceneManager::addObject("plane", SceneObject::ObjectType::PRIMITIVE, "plane");
  obj->transform.setPosition(0.0f,-0.5f,0.0f);
  obj->transform.setScale(10.0,10.0f,10.0f);
  SceneManager::addObject("teapot", SceneObject::ObjectType::PRIMITIVE, "teapot");

  //BASE
  createShaderProgram("PBR", "shaders/pbrVertex.glsl", "shaders/pbrFrag.glsl");
  createShaderProgram("Sprite", "shaders/spriteVertex.glsl", "shaders/spriteFrag.glsl");
  createShaderProgram("Unlit", "shaders/pbrVertex.glsl", "shaders/unlitFrag.glsl");

  //SHADOW MAPS
  createShaderProgram("shadowMap", "shaders/shadow/shadowVertex.glsl", "shaders/shadow/shadowFrag.glsl");
  createShaderProgram("shadowOmniMap", "shaders/shadow/omniShadowVertex.glsl", "shaders/shadow/omniShadowFrag.glsl",
                                                                  "shaders/shadow/omniShadowGeo.glsl");

  //DEFERRED
  createShaderProgram("Screen", "shaders/screenVertex.glsl", "shaders/screenFrag.glsl");
  createShaderProgram("gBufferPBR", "shaders/pbrVertex.glsl", "shaders/deferred/deferredFrag.glsl");
  createShaderProgram("DeferredPBR", "shaders/screenVertex.glsl", "shaders/deferred/deferredPBRFrag.glsl");


  // now a light
  // setup the default shader mate*m_v_transform.getMatrix()oughness",0.38f);
  // ngl::ShaderLib::setUniform("ao",0.2f);

  //ngl::VAOPrimitives::createSphere("sphere",1.0,40);

  m_text=std::make_unique<ngl::Text>("fonts/Arial.ttf",18);
  m_text->setScreenSize(this->size().width(),this->size().height());
  m_text->setColour(1.0,1.0,1.0);

  // std::unique_ptr<ngl::Obj> m_obj = std::make_unique<ngl::Obj>("../meshes/testcar.obj");
  // std::cout << std::endl << m_obj->getNumFaces() << " - num of faces of object";
  // if(m_obj->isLoaded())
  // {
  //     m_obj->createVAO();
  //     ngl::VAOPrimitives::addToPrimitives("test",std::move(m_obj->moveVAO()));
  // }

  auto tex = ngl::Texture("../textures/rustediron2_basecolor.png");
  m_test_tex_id = tex.setTextureGL();

  initOmniShadowMaps();
  initDeferred();
  updateLightInfo();


  QTimer *fpsTimer = new QTimer(this);
  connect(fpsTimer, &QTimer::timeout, this, [this]{update();});
  fpsTimer->start(1000/60);
}

void NGLScene::initDirShadowMaps()
{
    //Create shadow depth frame buffer
    glGenFramebuffers(1, &m_depthMapFBO);
    glGenTextures(1, &m_depthMap);
    glBindTexture(GL_TEXTURE_2D, m_depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
                m_SHADOW_WIDTH, m_SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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

void NGLScene::initOmniShadowMaps()
{
  glGenFramebuffers(1, &m_depthMapFBO);
  glGenTextures(1, &m_depthCubeMap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMap);

  for (unsigned int i = 0; i < 6; ++i)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, 
                     m_SHADOW_WIDTH, m_SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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
  glGenTextures(1, &m_gBufferPosition);
  glBindTexture(GL_TEXTURE_2D, m_gBufferPosition);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gBufferPosition, 0);

  // - normal color buffer
  glGenTextures(1, &m_gBufferNormal);
  glBindTexture(GL_TEXTURE_2D, m_gBufferNormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gBufferNormal, 0);
    
  // - albedo buffer
  glGenTextures(1, &m_gBufferAlbedo);
  glBindTexture(GL_TEXTURE_2D, m_gBufferAlbedo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gBufferAlbedo, 0);

  // - metallic roughness ao buffer
  glGenTextures(1, &m_gBufferAORoughnessMetallic);
  glBindTexture(GL_TEXTURE_2D, m_gBufferAORoughnessMetallic);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, m_gBufferAORoughnessMetallic, 0);
  
  //depth
  glGenTextures(1, &m_gBufferDepth);
  glBindTexture(GL_TEXTURE_2D, m_gBufferDepth);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_gBufferDepth, 0);

  // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
  unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
  glDrawBuffers(4, attachments);
  glEnable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  ngl::ShaderLib::use("DeferredPBR");
  //ngl::ShaderLib::getUniformBlockIndex("")
  ngl::ShaderLib::setUniform("gNormal",1);
  ngl::ShaderLib::setUniform("gAlbedo",2);
  ngl::ShaderLib::setUniform("gAORoughMet",3);
  ngl::ShaderLib::setUniform("gDepth",4);
}

void NGLScene::updateDeferredSize()
{
  auto size = this->size();
  int width = size.width();
  int height = size.height();

  glBindTexture(GL_TEXTURE_2D, m_gBufferPosition);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, m_gBufferNormal);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, m_gBufferAlbedo);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, m_gBufferAORoughnessMetallic);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
  glBindTexture(GL_TEXTURE_2D, m_gBufferDepth);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

void NGLScene::loadShaderDefaults()
{
  ngl::ShaderLib::use("PBR");
  ngl::ShaderLib::setUniform("albedo",0.950f, 0.71f, 0.29f);
  ngl::ShaderLib::setUniform("metallic",1.02f);
  ngl::ShaderLib::setUniform("roughness",0.38f);
  ngl::ShaderLib::setUniform("ao",0.2f);
}


void NGLScene::updateNumLights()
{
    int numLights = m_lights.size();
    m_lightInfoArray.resize(numLights);

    
    std::cout << "\nSetting num of lights to " << numLights;

    std::string shadersToEdit[] = {"pbrFrag", "deferredPBRFrag"};
    //std::string programNames[] = {"PBR"};

    for(int i=0;i<2;i++)
    {
        if(ngl::ShaderLib::getShader(shadersToEdit[i])!=nullptr)
        {
          auto &_shader = shadersToEdit[i];
          //auto &_shaderProgram = programNames[i];
          ngl::ShaderLib::resetEdits(_shader);
          ngl::ShaderLib::editShader(_shader,"@numLights",std::to_string(numLights));
          ngl::ShaderLib::compileShader(_shader);
        }
        
        //ngl::ShaderLib::use(_shaderProgram);
    }
    
    updateLightInfo();
};

void NGLScene::updateLightInfo()
{
    int i=0;
    for (auto& it: m_lights)
    {
        m_lightInfoArray[i].col = it.second->getColour();
        m_lightInfoArray[i].pos = it.second->transform.getPosition();
        m_lightInfoArray[i].intensity = it.second->getIntensity();
        i++;
    }
    loadLightsToShader();
}


void NGLScene::loadLightsToShader()
{   
    std::string shaderName = ngl::ShaderLib::getCurrentShaderName();
    std::string lightShaders[] = {"PBR", "DeferredPBR"};

    if( std::find(std::begin(lightShaders), std::end(lightShaders), shaderName) != std::end(lightShaders))
    {
      ngl::ShaderLib::linkProgramObject(shaderName);
      ngl::ShaderLib::use(shaderName);
      loadMatricesToShader();
      //ngl::ShaderLib::printRegisteredUniforms("PBR");
      for(int i=0;i<m_lightInfoArray.size();i++)    //Create shadow depth frame buffer
      {
          ngl::ShaderLib::setUniform(fmt::format("lightColours[{0}]",i),m_lightInfoArray[i].col);
          ngl::ShaderLib::setUniform(fmt::format("lightPositions[{0}]",i),m_lightInfoArray[i].pos);
          ngl::ShaderLib::setUniform(fmt::format("lightInts[{0}]",i),m_lightInfoArray[i].intensity);
      }
    }
    
}

void NGLScene::createShaderProgram(const std::string &_name, const std::string &_vertPath, const std::string &_fragPath,const std::string &_geoPath)
{
  std::string vertName, fragName, geoName;
  //ugly syntax - im sorry
  ngl::ShaderLib::createShaderProgram( _name );

  vertName = QString(_vertPath.c_str()).split("/").last().split(".").first().toStdString();
  ngl::ShaderLib::attachShader( vertName, ngl::ShaderType::VERTEX );
  ngl::ShaderLib::loadShaderSource( vertName, _vertPath );

  fragName = QString(_fragPath.c_str()).split("/").last().split(".").first().toStdString();
  ngl::ShaderLib::attachShader( fragName, ngl::ShaderType::FRAGMENT );
  ngl::ShaderLib::loadShaderSource( fragName, _fragPath );

  if(_geoPath!="")
  {
      geoName = QString(_geoPath.c_str()).split("/").last().split(".").first().toStdString();
      ngl::ShaderLib::attachShader( geoName, ngl::ShaderType::GEOMETRY );
      ngl::ShaderLib::loadShaderSource( geoName, _geoPath );
  }

  std::cout << "\nCreating shader " << _name << " with vertex shader " << _vertPath << vertName << " and fragment shader " << _fragPath << fragName;
  
  updateNumLights();

  ngl::ShaderLib::compileShader( vertName );
  ngl::ShaderLib::attachShaderToProgram( _name, vertName );

  ngl::ShaderLib::compileShader( fragName );
  ngl::ShaderLib::attachShaderToProgram( _name, fragName );

  if(_geoPath!="")
  {
    ngl::ShaderLib::compileShader( geoName );
    ngl::ShaderLib::attachShaderToProgram( _name, geoName );
  }

  ngl::ShaderLib::linkProgramObject( _name );
  ngl::ShaderLib::use(_name);
  //ngl::ShaderLib::setUniform( "camPos", eye );
}


void NGLScene::resizeGL( int _w, int _h )
{
  m_project=ngl::perspective( 45.0f, static_cast<float>( _w ) / _h, 0.05f, 350.0f );
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  updateDeferredSize();
}

transform NGLScene::getViewProjection()
{
    m_VP.V = m_v_trans*m_view*m_v_rot*m_v_scale;
    m_VP.P = m_project;
    m_VP.VP = m_VP.P * m_VP.V;
    return m_VP;
}

void NGLScene::loadMatricesToShader()
{
    getViewProjection();

    ngl::ShaderLib::setUniformBuffer("TransformVP",sizeof(transform),&m_VP.V.m_00);
    auto camPos = m_VP.V.inverse();
    auto _name = ngl::ShaderLib::getCurrentShaderName();
    if(_name=="PBR" || _name=="DeferredPBR")
    {
      //loadDirShadowMatrices();
      ngl::ShaderLib::setUniform( "camPos", camPos.m_30, camPos.m_31, camPos.m_32 );
    }
      
}

void NGLScene::paintGL()
{
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_mainFBO);
    //glCullFace(GL_BACK);
    renderOmniShadowMaps();
    //glCullFace(GL_FRONT);

    renderDeferred();

    glBindFramebuffer(GL_FRAMEBUFFER, m_mainFBO);
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

void NGLScene::renderDeferred()
{
  glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
  glViewport(0,0,m_win.width,m_win.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SceneManager::draw("gBufferPBR");

  glBindFramebuffer(GL_FRAMEBUFFER, m_mainFBO);
    ngl::ShaderLib::use("DeferredPBR");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gBufferDepth);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gBufferNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gBufferAlbedo);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_gBufferAORoughnessMetallic);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_gBufferDepth);

    transform t = getViewProjection();
    t.VP = t.VP.inverse();
    ngl::ShaderLib::setUniformBuffer("InverseVP",sizeof(transform),&t.V.m_00);

    ngl::VAOPrimitives::draw("screenQuad");


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

  ngl::Vec3 lightPos = m_omniLight->transform.getPosition();

  float aspect = (float)m_SHADOW_WIDTH/(float)m_SHADOW_HEIGHT;
  float near = 1.0f;
  float far = 25.0f;
  ngl::Mat4 shadowProj = ngl::perspective(90.0f, aspect, near, far); 

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
  ngl::ShaderLib::setUniform("far_plane",far);
  ngl::ShaderLib::use("PBR");
  ngl::ShaderLib::setUniform("lightPos",lightPos);
  ngl::ShaderLib::setUniform("far_plane", far);
  ngl::ShaderLib::use("DeferredPBR");
  ngl::ShaderLib::setUniform("lightPos",lightPos);
  ngl::ShaderLib::setUniform("far_plane", far);

}

void NGLScene::renderDirShadowMaps()
{
    GLint prev_fbo;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbo);

    glViewport(0, 0, m_SHADOW_WIDTH, m_SHADOW_HEIGHT);
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

    glViewport(0, 0, m_SHADOW_WIDTH, m_SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, m_depthMapFBO);
      glClear(GL_DEPTH_BUFFER_BIT);
      glEnable(GL_DEPTH_TEST);
      //glActiveTexture(GL_TEXTURE0);
      //glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMap);
      loadOmniShadowMatrices();
      SceneManager::draw("shadowOmniMap");

    glBindFramebuffer(GL_FRAMEBUFFER, prev_fbo);
}

void NGLScene::setViewToSelected()
{
    m_v_rot.identity();
    m_v_trans.identity();
    m_v_scale.identity();

    auto new_pos = SceneManager::getSelectedObject()->transform.getPosition();
}

NGLScene::~NGLScene()
{
}

// void NGLScene::setColour()
// {
// 	QColor colour = QColorDialog::getColor();
// 	if( colour.isValid())
// 	{
    
//     ngl::ShaderLib::use("PBR");
//     ngl::ShaderLib::setUniform("albedo",static_cast<float>(colour.redF()),static_cast<float>(colour.greenF()),static_cast<float>(colour.blueF()));
//     update();
// 	}
// }

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
