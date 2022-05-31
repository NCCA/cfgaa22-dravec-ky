#ifndef NGLSCENE_H_
#define NGLSCENE_H_

#include <ngl/Transformation.h>
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include <ngl/Text.h>
#include "WindowParams.h"
#include <QEvent>
#include <QResizeEvent>
#include <QOpenGLWidget>
#include <memory>
#include <ngl/ShaderLib.h>
#include "SceneMesh.h"
#include "SceneObject.h"
#include "SceneLight.h"
#include "RenderParams.h"

/// @file NGLScene.h
/// @brief a basic Qt GL window class for ngl demos
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/10/10
/// Revision History :
/// Initial Version 10/10/10 (Binary day ;-0 )
/// @class GLWindow
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file

struct LightInfo
{
	ngl::Vec3 pos;
	ngl::Vec3 col;
	float intensity;
};

struct transform
  {
    ngl::Mat4 V;
    ngl::Mat4 P;
    ngl::Mat4 VP;
  };

class NGLScene : public QOpenGLWidget
{
  Q_OBJECT
public :

  NGLScene(QWidget *_parent );

  ~NGLScene() override;

  void loadMatricesToShader();

	void updateNumLights();
	void updateLightInfo();
	void loadLightsToShader(const std::string &_shaderName = ngl::ShaderLib::getCurrentShaderName());


	void loadShaderDefaults(const std::string &_shaderName = ngl::ShaderLib::getCurrentShaderName());
	void setViewToSelected();
  transform getViewProjection();

  //SHADOW MAPPING
  void initDirShadowMaps();
  void initOmniShadowMaps();

  void loadDirShadowMatrices();
  void loadOmniShadowMatrices();

  void updateDirShadowSize();
  void updateOmniShadowSize();

  void renderDirShadowMaps();
  void renderOmniShadowMaps();

  //DEFERRED RENDERING
  void initDeferred();
  void updateDeferredSize();

  bool setRenderFunction(void (NGLScene::*func)() );
  void renderForward();
  void renderWireframe();

  void renderDeferred();
    void renderDeferredAlbedo();
    void renderDeferredORM();
    void renderDeferredNormal();
    void renderDeferredEmissive();
    
    void renderShadowMap();

  void renderTexture(const GLuint &tex_id);

  //ASSETS
  void initDefaultAssets();
  void loadScene1();
  void loadSceneCornell();
  void loadSceneSponza();
  void loadScenePBR();
  void loadSceneLights();

  /// @brief function for loading shaders onto the ngl::ShaderLib
  void createShaderProgram(const std::string &_name, const std::string &_vertPath, const std::string &_fragPath, const std::string &_geoPath = "");
  /// @brief unordered map of all scene lights

  std::unordered_map<int,std::shared_ptr<SceneLight>> m_lights;

  const unsigned int m_SHADOW_WIDTH = 1024, m_SHADOW_HEIGHT = 1024;
  
  //DEFAULT PBR TEXTURES
  GLuint m_DefaultAlbedo;
  GLuint m_DefaultAORoughMet;
  GLuint m_DefaultNormal;
  GLuint m_DefaultEmissive;

  RenderParams m_params;

private:
	/// @brief m_wireframe mode
	bool m_wireframe = false;
	/// @brief rotation data
  ngl::Vec3 m_rotation;
	/// @brief scale data
  ngl::Vec3 m_scale;
	/// @brief position data
  ngl::Vec3 m_position;
	/// @brief our object to draw
	//----------------------------------------------------------------------------------------------------------------------
	// text for rendering
	//----------------------------------------------------------------------------------------------------------------------
	std::unique_ptr<ngl::Text> m_text;

  transform m_VP;

  void (NGLScene::*f_executeRender)();
  
protected:
  /// @brief  The following methods must be implimented in the sub class
  /// this is called when the window is created
  void initializeGL() override;

  /// @brief this is called whenever the window is re-sized
  /// @param[in] _w the width of the resized window
  /// @param[in] _h the height of the resized window
  void resizeGL(int _w , int _h) override;

  /// @brief this is the main gl drawing routine which is called whenever the window needs to be redrawn
  void paintGL() override;

  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the windows params such as mouse and rotations etc
  //----------------------------------------------------------------------------------------------------------------------
  WinParams m_win;
  

  /// @brief Camera start location matrix
  ngl::Mat4 m_view;

	/// Transformation matrices for moving the camera 
  ngl::Mat4 m_v_rot;
  ngl::Mat4 m_v_scale;
  ngl::Mat4 m_v_trans;

  //Projection matrix
  ngl::Mat4 m_project;

private :
  //MOUSE EVENTS
  void mouseMoveEvent (QMouseEvent * _event   ) override;
  void mousePressEvent ( QMouseEvent *_event  ) override;
  void mouseReleaseEvent (QMouseEvent *_event ) override;
  void wheelEvent( QWheelEvent* _event ) override;
  void keyPressEvent(QKeyEvent * _event) override;


  void createScreenQuad();

  //SHADOW PASS
  GLint m_mainFBO;

  
  GLuint m_depthMapFBO;
  GLuint m_depthMap;

  GLuint m_depthCubeMapFBO;
  GLuint m_depthCubeMap;


  std::shared_ptr<SceneObject> m_omniLight;
	std::vector<LightInfo> m_lightInfoArray;

  //DEFERRED RENDERING
  GLuint m_gBuffer;
  //GLuint m_gBufferPosition;
  GLuint m_gBufferNormal;
  GLuint m_gBufferAlbedo;
  GLuint m_gBufferAORoughnessMetallic;
  GLuint m_gBufferDepth;
  GLuint m_gBufferEmissive;


  //TEMP VARS
	std::shared_ptr<SceneObject> m_mesh;
  GLuint m_test_tex_id;
  
};

#endif
