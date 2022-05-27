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
#include "SceneMesh.h"
#include "SceneObject.h"
#include "SceneLight.h"

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
Q_OBJECT        // must include this if you use Qt signals/slots
public :
  /// @brief Constructor for GLWindow
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Constructor for GLWindow
  /// @param [in] _parent the parent window to create the GL context in
  //----------------------------------------------------------------------------------------------------------------------
  NGLScene(QWidget *_parent );

		/// @brief dtor
  ~NGLScene() override;
  	void loadMatricesToShader();
	

  	std::unordered_map<int,std::shared_ptr<SceneLight>> m_lights;
	void updateNumLights();
	void updateLightInfo();
	void loadLightsToShader();

	void loadShaderDefaults();
	void setViewToSelected();
  transform getViewProjection();

  void initDirShadowMaps();
  void renderDirShadowMaps();
  void loadDirShadowMatrices();

  void initOmniShadowMaps();
  void renderOmniShadowMaps();
  void loadOmniShadowMatrices();
	

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

protected:
  void createShaderProgram(const std::string &_name, const std::string &_vertPath, const std::string &_fragPath, const std::string &_geoPath = "");
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the windows params such as mouse and rotations etc
  //----------------------------------------------------------------------------------------------------------------------
  WinParams m_win;
  /// @brief  The following methods must be implimented in the sub class
  /// this is called when the window is created
  void initializeGL() override;

  /// @brief this is called whenever the window is re-sized
  /// @param[in] _w the width of the resized window
  /// @param[in] _h the height of the resized window
  void resizeGL(int _w , int _h) override;
  /// @brief this is the main gl drawing routine which is called whenever the window needs to
  // be re-drawn
  void paintGL() override;

  /// @brief our model position
  ngl::Vec3 m_modelPos;
  /// @brief our camera
  ngl::Mat4 m_view;

	//Transformation matrix for moving the camera + variables for temporarily storing increments during event inputs
  ngl::Transformation m_v_transform;
  ngl::Mat4 m_v_rot;
  ngl::Mat4 m_v_scale;
  ngl::Mat4 m_v_trans;

  ngl::Mat4 m_project;
	/// @brief our transform for objects
  ngl::Transformation m_transform;
private :
  /// @brief this method is called every time a mouse is moved
  /// @param _event the Qt Event structure

  void mouseMoveEvent (QMouseEvent * _event   ) override;
  /// @brief this method is called everytime the mouse button is pressed
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure

  void mousePressEvent ( QMouseEvent *_event  ) override;

  /// @brief this method is called everytime the mouse button is released
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  void mouseReleaseEvent (QMouseEvent *_event ) override;
  void wheelEvent( QWheelEvent* _event ) override;
  void keyPressEvent(QKeyEvent * _event) override;

  GLint m_mainFBO;

  const unsigned int m_SHADOW_WIDTH = 1024, m_SHADOW_HEIGHT = 1024;
  GLuint m_depthMapFBO;
  GLuint m_depthMap;

  GLuint m_depthCubeMapFBO;
  GLuint m_depthCubeMap;

  


  //std::unique_ptr<ngl::AbstractVAO> m_vao;
	std::shared_ptr<SceneObject> m_mesh;
  std::shared_ptr<SceneObject> m_omniLight;

	std::vector<LightInfo> m_lightInfoArray;
	

};

#endif
