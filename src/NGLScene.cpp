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
  initDefaultAssets();

  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);			   // Grey Background
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_LINE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

  m_project=ngl::perspective(45,float(1024/720),0.1f,300.0f);

  loadSceneCornell();

  m_text=std::make_unique<ngl::Text>("fonts/Arial.ttf",18);
  m_text->setScreenSize(this->size().width(),this->size().height());
  m_text->setColour(1.0,1.0,1.0);

  initOmniShadowMaps();
  //initDirShadowMaps();
  updateLightInfo();

  //BASE
  createShaderProgram("PBR", "shaders/pbrVertex.glsl", "shaders/pbrFrag.glsl");
  createShaderProgram("Unlit", "shaders/pbrVertex.glsl", "shaders/unlitFrag.glsl");

  //SHADOW MAPS
  createShaderProgram("shadowMap", "shaders/shadow/shadowVertex.glsl", "shaders/shadow/shadowFrag.glsl");
  createShaderProgram("shadowOmniMap", "shaders/shadow/omniShadowVertex.glsl", "shaders/shadow/omniShadowFrag.glsl",
                                                                  "shaders/shadow/omniShadowGeo.glsl");

  //DEFERRED
  createShaderProgram("Screen", "shaders/screenVertex.glsl", "shaders/screenFrag.glsl");
  createShaderProgram("gBufferPBR", "shaders/pbrVertex.glsl", "shaders/deferred/deferredFrag.glsl");
  createShaderProgram("DeferredPBR", "shaders/screenVertex.glsl", "shaders/deferred/deferredPBRFrag.glsl");


  initDeferred();

  QTimer *fpsTimer = new QTimer(this);
  connect(fpsTimer, &QTimer::timeout, this, [this]{update();});
  fpsTimer->start(1000/120);

  SceneManager::setRenderFunction(&NGLScene::renderForward);

}
void NGLScene::updateNumLights()
{
    int numLights = (int)m_lights.size();
    numLights = std::max(numLights,1);
    m_lightInfoArray.resize(numLights);

    
    std::cout << "\nSetting num of lights to " << numLights;

    std::string shadersToEdit[] = {"pbrFrag", "deferredPBRFrag"};

    for(int i=0;i<2;i++)
    {
        if(ngl::ShaderLib::getShader(shadersToEdit[i])!=nullptr)
        {
          auto &_shader = shadersToEdit[i];
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
    if(m_lights.size()==0) m_lightInfoArray[0].col = ngl::Vec3(0);
    else
    {
      for (auto& it: m_lights)
      {
          m_lightInfoArray[i].col = it.second->getColour();
          m_lightInfoArray[i].pos = it.second->transform.getPosition();
          m_lightInfoArray[i].intensity = it.second->getIntensity();
          i++;
      }
    }
    loadLightsToShader();
}


void NGLScene::loadLightsToShader(const std::string &_shaderName)
{   
    std::string lightShaders[] = {"PBR", "DeferredPBR"};

    if( std::find(std::begin(lightShaders), std::end(lightShaders), _shaderName) != std::end(lightShaders))
    {
      ngl::ShaderLib::linkProgramObject(_shaderName);
      ngl::ShaderLib::use(_shaderName);
      loadShaderDefaults();
      loadMatricesToShader();
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
  ngl::ShaderLib::createShaderProgram( _name );

  //ugly syntax - im sorry
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
  loadShaderDefaults();
}


void NGLScene::resizeGL( int _w, int _h )
{
  m_project=ngl::perspective( m_params.CAM_FOV, static_cast<float>( _w ) / _h, 0.05f, 350.0f );
  m_params.WIDTH  = static_cast<int>( _w * devicePixelRatio() );
  m_params.HEIGHT = static_cast<int>( _h * devicePixelRatio() );
  updateDeferredSize();
}

transform NGLScene::getViewProjection()
{
    m_project=ngl::perspective( m_params.CAM_FOV, static_cast<float>( m_params.WIDTH ) / m_params.HEIGHT, 0.05f, 350.0f );
    
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
      ngl::ShaderLib::setUniform( "camPos", camPos.m_30, camPos.m_31, camPos.m_32 );
    }
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