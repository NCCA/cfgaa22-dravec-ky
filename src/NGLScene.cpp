#include "NGLScene.h"
#include "SceneManager.h"
#include "Utils.h"
#include <iostream>
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
  ngl::VAOPrimitives::createTrianglePlane("screenQuad",2,2,1,1,ngl::Vec3(0,1,0));
  ngl::VAOPrimitives::createTorus("torus",0.3,1,8,16);
  ngl::VAOPrimitives::createCylinder("cylinder",0.8,2,16,3);
  ngl::VAOPrimitives::createSphere("sphere",1,16);

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

  createShaderProgram("PBR", "shaders/pbrVertex.glsl", "shaders/pbrFrag.glsl");
  createShaderProgram("Sprite", "shaders/spriteVertex.glsl", "shaders/spriteFrag.glsl");
  createShaderProgram("Unlit", "shaders/pbrVertex.glsl", "shaders/unlitFrag.glsl");
  createShaderProgram("shadowMap", "shaders/shadowVertex.glsl", "shaders/shadowFrag.glsl");
  createShaderProgram("Screen", "shaders/screenVertex.glsl", "shaders/screenFrag.glsl");
  createShaderProgram("shadowOmniMap", "shaders/omniShadowVertex.glsl", "shaders/omniShadowFrag.glsl",
                                                                  "shaders/omniShadowGeo.glsl");

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
  static GLint result;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &result);
  std::cout << "!!!!!!!!!!!!!!!!!" <<result;
  initOmniShadowMaps();
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &result);
  std::cout << "!!!!!!!!!!!!!!!!!" <<result;

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

    std::string shadersToEdit[] = {"pbrFrag"};
    std::string programNames[] = {"PBR"};

    for(int i=0;i<1.;i++)
    {
        if(ngl::ShaderLib::getShader(shadersToEdit[i])!=nullptr)
        {
          auto &_shader = shadersToEdit[i];
          auto &_shaderProgram = programNames[i];
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

        std::cout << fmt::format("\n{}\nCol: {} {} {}\nPos: {} {} {}\nInt: {}\n",
                                  i,
                                  m_lightInfoArray[i].col[0],
                                  m_lightInfoArray[i].col[1],
                                  m_lightInfoArray[i].col[2],
                                  m_lightInfoArray[i].pos[0],
                                  m_lightInfoArray[i].pos[1],
                                  m_lightInfoArray[i].pos[2],
                                  m_lightInfoArray[i].intensity);
        i++;
    }
    loadLightsToShader();
}


void NGLScene::loadLightsToShader()
{   
    ngl::ShaderLib::linkProgramObject("PBR");
    ngl::ShaderLib::use("PBR");
    loadMatricesToShader();
    //ngl::ShaderLib::printRegisteredUniforms("PBR");
    for(int i=0;i<m_lightInfoArray.size();i++)    //Create shadow depth frame buffer
    {
        ngl::ShaderLib::setUniform(fmt::format("lightColours[{0}]",i),m_lightInfoArray[i].col);
        ngl::ShaderLib::setUniform(fmt::format("lightPositions[{0}]",i),m_lightInfoArray[i].pos);
        ngl::ShaderLib::setUniform(fmt::format("lightInts[{0}]",i),m_lightInfoArray[i].intensity);
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
  //  struct transform
  //  {
  //    ngl::Mat4 MVP;
  //    ngl::Mat4 normalMatrix;
  //    ngl::Mat4 M;
  //  };
  //  transform t;

    //Utils::printMatrix(t.M);
    getViewProjection();
    //auto matrix = m_project*m_v_trans*m_view*m_v_rot*m_v_scale;

    ngl::ShaderLib::setUniformBuffer("TransformVP",sizeof(transform),&m_VP.V.m_00);
    auto camPos = m_VP.V.inverse();
    if(ngl::ShaderLib::getCurrentShaderName()=="PBR")
    {
      //loadDirShadowMatrices();
      ngl::ShaderLib::setUniform( "camPos", camPos.m_30, camPos.m_31, camPos.m_32 );
    }
      
}

void NGLScene::paintGL()
{
    //glCullFace(GL_FRONT);
    renderOmniShadowMaps();
    //glCullFace(GL_BACK);

    glViewport(0,0,m_win.width,m_win.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
    // glEnable(GL_DEPTH_TEST);
    ngl::ShaderLib::use("PBR");
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, m_depthMap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_depthCubeMap);
    //loadMatricesToShader();
    SceneManager::draw("PBR");


    // glActiveTexture(GL_TEXTURE0);
    // glBindTexture(GL_TEXTURE_2D, m_depthMap);
    // ngl::VAOPrimitives::draw("screenQuad");

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
