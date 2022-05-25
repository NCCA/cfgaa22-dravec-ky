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

constexpr auto shaderProgram="PBR";

void NGLScene::initializeGL()
{

  ngl::NGLInit::initialize();
  //prepare some basic shapes
  ngl::VAOPrimitives::createTrianglePlane("plane",1,1,0,1,ngl::Vec3(0,1,0));
  ngl::VAOPrimitives::createTorus("torus",0.3,1,8,16);
  ngl::VAOPrimitives::createCylinder("cylinder",0.8,2,16,3);

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
  constexpr auto vertexShader  = "ColVertex";
  constexpr auto fragShader    = "ColFrag";
  // create the shader program
  ngl::ShaderLib::createShaderProgram( shaderProgram );
  // now we are going to create empty shaders for Frag and Vert
  ngl::ShaderLib::attachShader( vertexShader, ngl::ShaderType::VERTEX );
  ngl::ShaderLib::attachShader( fragShader, ngl::ShaderType::FRAGMENT );
  // attach the source
  ngl::ShaderLib::loadShaderSource( vertexShader, "shaders/ColVertex.glsl" );
  ngl::ShaderLib::loadShaderSource( fragShader, "shaders/ColFrag.glsl" );
  // compile the shaders
  ngl::ShaderLib::compileShader( vertexShader );
  ngl::ShaderLib::compileShader( fragShader );
  // add them to the program
  ngl::ShaderLib::attachShaderToProgram( shaderProgram, vertexShader );
  ngl::ShaderLib::attachShaderToProgram( shaderProgram, fragShader );
  // now we have associated that data we can link the shader
  ngl::ShaderLib::linkProgramObject( shaderProgram );
  // and make it active ready to load values
  ngl::ShaderLib::use( shaderProgram );
  //ngl::ShaderLib::setUniform( "camPos", eye );

  // now a light
  // setup the default shader mate*m_v_transform.getMatrix()oughness",0.38f);
  // ngl::ShaderLib::setUniform("ao",0.2f);

  //ngl::VAOPrimitives::createSphere("sphere",1.0,40);

  m_text=std::make_unique<ngl::Text>("fonts/Arial.ttf",18);
  m_text->setScreenSize(this->size().width(),this->size().height());
  m_text->setColour(1.0,1.0,1.0);
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
  ngl::ShaderLib::use("PBR");
  loadMatricesToShader();

  SceneManager::draw();

	m_text->renderText(10,580,"Qt Gui Demo");
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
    
    ngl::ShaderLib::use(shaderProgram);
    ngl::ShaderLib::setUniform("albedo",static_cast<float>(colour.redF()),static_cast<float>(colour.greenF()),static_cast<float>(colour.blueF()));
    update();
	}
}
