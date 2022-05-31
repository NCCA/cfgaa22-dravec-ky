#include "NGLScene.h"
#include <QMouseEvent>
#include <iostream>
#include <ngl/Transformation.h>
#include <ngl/Vec3.h>
#include <ngl/Util.h>
#include "Utils.h"
#include "SceneManager.h"


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseMoveEvent( QMouseEvent* _event )
{
  // note the method buttons() is the button state when event was called
  // that is different from button() which is used to check which button was
  // pressed when the mousePress/Release event is generated
  if (_event->buttons() == Qt::LeftButton )
  {
    int diffx = _event->x() - m_params.origX;
    int diffy = _event->y() - m_params.origY;

    
    ngl::Mat4 temp_rot;

    temp_rot.identity();
    temp_rot.rotateY(diffx);
    m_v_rot = m_v_rot * temp_rot;

    temp_rot.identity();
    temp_rot.rotateX(diffy);
    m_v_rot = temp_rot * m_v_rot;



    m_params.origX = _event->x();
    m_params.origY = _event->y();
    update();
  }
  // right mouse translate code
  else if (_event->buttons() == Qt::RightButton )
  {
    int diffX      = static_cast<int>( _event->x() - m_params.origXPos );
    int diffY      = static_cast<int>( _event->y() - m_params.origYPos );
    m_params.origXPos = _event->x();
    m_params.origYPos = _event->y();

    ngl::Mat4 temp_trans;
    temp_trans.identity();
    temp_trans.m_30 += diffX/100.0f;
    temp_trans.m_31 -= diffY/100.0f;
    m_v_trans = temp_trans * m_v_trans;
    // m_modelPos.m_x += INCREMENT * diffX;
    // m_modelPos.m_y -= INCREMENT * diffY;
    update();
  }
}


//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mousePressEvent( QMouseEvent* _event )
{
  // that method is called when the mouse button is pressed in this case we
  // store the value where the maouse was clicked (x,y) and set the Rotate flag to true
  //std::cout << "mouse pressed";
  if ( _event->button() == Qt::LeftButton )
  {
    m_params.origX  = _event->x();
    m_params.origY  = _event->y();
  }
  // right mouse translate mode
  else if ( _event->button() == Qt::RightButton )
  {
    m_params.origXPos  = _event->x();
    m_params.origYPos  = _event->y();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::mouseReleaseEvent( QMouseEvent* _event )
{
  // that event is called when the mouse button is released
  // we then set Rotate to false
  if ( _event->button() == Qt::LeftButton )
  {
    m_params.rotate = false;
  }
  // right mouse translate mode
  if ( _event->button() == Qt::RightButton )
  {
    m_params.translate = false;
  }
}

void NGLScene::keyPressEvent(QKeyEvent *_event)
{

}

//----------------------------------------------------------------------------------------------------------------------
void NGLScene::wheelEvent( QWheelEvent* _event )
{
  auto ZOOM = _event->angleDelta().y()*0.001;
  // check the diff of the wheel position (0 means no change)
  if ( ZOOM != 0)
  {
    ngl::Mat4 temp_scale;
    temp_scale.identity();

    temp_scale *= (1+ZOOM);
    m_v_scale *= temp_scale;
    m_v_scale.m_33 = 1;

    //Utils::printMatrix(m_v_scale);
  }
  update();
}

