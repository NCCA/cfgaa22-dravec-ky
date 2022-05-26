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

  ngl::Vec3 eye(0.0f,2.0f,2.0f);
  ngl::Vec3 look(0,0,0);
  ngl::Vec3 up(0,1,0);
  m_view = ngl::lookAt(eye,look,up);

  m_project=ngl::perspective(45,float(1024/720),0.1f,300.0f);

  // in the code create some constexpr
  
  // create the shader program

  SceneManager::addLight(ngl::Vec3(0,3,0));

  createShaderProgram("PBR", "shaders/ColVertex.glsl", "shaders/ColFrag.glsl");
  createShaderProgram("Sprite", "shaders/spriteVertex.glsl", "shaders/spriteFrag.glsl");
  createShaderProgram("Unlit", "shaders/ColVertex.glsl", "shaders/unlitFrag.glsl");

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

  auto spaceship = SceneManager::addObject("spaceship",SceneObject::ObjectType::MESH, "../meshes/spaceship.obj");
  spaceship->transform.setScale(0.1,0.1,0.1);
  

}

void NGLScene::loadShaderDefaults()
{
  ngl::ShaderLib::use("PBR");
  ngl::ShaderLib::setUniform("albedo",0.950f, 0.71f, 0.29f);
  ngl::ShaderLib::setUniform("metallic",1.02f);
  ngl::ShaderLib::setUniform("roughness",0.38f);
  ngl::ShaderLib::setUniform("ao",0.2f);
}

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
}

void NGLScene::updateShaderLights()
{
    int numLights = m_lights.size();
    m_lightInfoArray.resize(numLights);

    updateLightInfo();
    std::cout << "\nSetting num of lights to " << numLights;

    std::string shadersToEdit[] = {"ColFrag"};
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
};

void NGLScene::loadLightsToShader()
{   
    ngl::ShaderLib::linkProgramObject("PBR");
    ngl::ShaderLib::use("PBR");
    //ngl::ShaderLib::printRegisteredUniforms("PBR");
    for(int i=0;i<m_lightInfoArray.size();i++)
    {
        ngl::ShaderLib::setUniform(fmt::format("lightColours[{0}]",i),m_lightInfoArray[i].col);
        ngl::ShaderLib::setUniform(fmt::format("lightPositions[{0}]",i),m_lightInfoArray[i].pos);
        ngl::ShaderLib::setUniform(fmt::format("lightInts[{0}]",i),m_lightInfoArray[i].intensity);
    }
}

void NGLScene::createShaderProgram(const std::string &_name, const std::string &_vertPath, const std::string &_fragPath)
{
  //ugly syntax - im sorry
  auto vertName = QString(_vertPath.c_str()).split("/").last().split(".").first().toStdString();
  auto fragName = QString(_fragPath.c_str()).split("/").last().split(".").first().toStdString();

  std::cout << "\nCreating shader " << _name << " with vertex shader " << _vertPath << vertName << " and fragment shader " << _fragPath << fragName;
  ngl::ShaderLib::createShaderProgram( _name );
  // now we are going to create empty shaders for Frag and Vert
  ngl::ShaderLib::attachShader( vertName, ngl::ShaderType::VERTEX );
  ngl::ShaderLib::attachShader( fragName, ngl::ShaderType::FRAGMENT );
  // attach the source
  ngl::ShaderLib::loadShaderSource( vertName, _vertPath );
  ngl::ShaderLib::loadShaderSource( fragName, _fragPath );
  updateShaderLights();
  // compile the shaders
  ngl::ShaderLib::compileShader( vertName );
  ngl::ShaderLib::compileShader( fragName );
  // add them to the program
  ngl::ShaderLib::attachShaderToProgram( _name, vertName );
  ngl::ShaderLib::attachShaderToProgram( _name, fragName );
  // now we have associated that data we can link the shader
  ngl::ShaderLib::linkProgramObject( _name );
  // and make it active ready to load values
  ngl::ShaderLib::use(_name);
  //ngl::ShaderLib::setUniform( "camPos", eye );
}


void NGLScene::resizeGL( int _w, int _h )
{
  m_project=ngl::perspective( 45.0f, static_cast<float>( _w ) / _h, 0.05f, 350.0f );
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
}


void NGLScene::loadMatricesToShader()
{
   struct transform
   {
     ngl::Mat4 MVP;
     ngl::Mat4 normalMatrix;
     ngl::Mat4 M;
   };
   transform t;

    t.M=m_transform.getMatrix();
    //Utils::printMatrix(t.M);
    t.MVP=m_project*m_v_trans*m_view*m_v_rot*m_v_scale*t.M;
    //Utils::printMatrix(t.MVP);
    t.normalMatrix.inverse().transpose();
    ngl::ShaderLib::setUniformBuffer("TransformUBO",sizeof(transform),&t.MVP.m_00);
    ngl::ShaderLib::setUniform( "camPos", t.MVP.m_30, t.MVP.m_31, t.MVP.m_32 );
}

void NGLScene::paintGL()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);
  
  if(m_wireframe == true)
  {
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  }
  else
  {
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  }

  ngl::Mat4 matrix;
  matrix.identity();

  loadMatricesToShader();
  loadLightsToShader();
  loadShaderDefaults();

  ngl::ShaderLib::setUniformMatrix4fv("inTransform",&matrix.m_00);

  // m_vao->bind();
  // m_vao->draw();
  // m_vao->unbind();
  
  //m_mesh->draw();
  SceneManager::draw();
  //SceneManager::addObject("mesh",SceneObject::ObjectType::MESH, "path");
	//m_text->renderText(10,580,"Qt Gui Demo");
}


NGLScene::~NGLScene()
{
}

void NGLScene::toggleWireframe(bool _mode	 )
{
	m_wireframe=_mode;
	update();
}

void NGLScene::setXRotation( double _x		)
{
	m_rotation.m_x=_x;
	update();
}

void NGLScene::setYRotation( double _y	)
{
	m_rotation.m_y=_y;
	update();
}
void NGLScene::setZRotation( double _z )
{
	m_rotation.m_z=_z;
	update();
}

void NGLScene::setXScale( double _x	)
{
	m_scale.m_x=_x;
	update();
}

void NGLScene::setYScale(	 double _y)
{
	m_scale.m_y=_y;
	update();
}
void NGLScene::setZScale( double _z )
{
	m_scale.m_z=_z;
	update();
}

void NGLScene::setXPosition( double _x	)
{
	m_position.m_x=_x;
	update();
}

void NGLScene::setYPosition( double _y	)
{
	m_position.m_y=_y;
	update();
}
void NGLScene::setZPosition( double _z	 )
{
	m_position.m_z=_z;
	update();
}

void NGLScene::setObjectMode(	int _i)
{
	update();
}
void NGLScene::setColour()
{
	QColor colour = QColorDialog::getColor();
	if( colour.isValid())
	{
    
    ngl::ShaderLib::use("PBR");
    ngl::ShaderLib::setUniform("albedo",static_cast<float>(colour.redF()),static_cast<float>(colour.greenF()),static_cast<float>(colour.blueF()));
    update();
	}
}
