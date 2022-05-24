#ifndef NGLOBJECTMENU_H_
#define NGLOBJECTMENU_H_

#include <QGroupBox>
#include <QSpinBox>
#include "SceneObject.h"

//base class definition from https://doc.qt.io/archives/4.6/model-view-creating-models.html
class NGLObjectMenu : public QGroupBox
{
    Q_OBJECT
  public:
    NGLObjectMenu(QWidget *parent = nullptr);
    ~NGLObjectMenu() {};
    void updateObject(std::shared_ptr<SceneObject> _obj);
    void setTransforms();
    void setTranslate(int _i, double _val);
    void setRotate(int _i, double _val);
    void setScale(int _i, double _val);
  public slots:

  private:
    QDoubleSpinBox * m_translateX = new QDoubleSpinBox;
    QDoubleSpinBox * m_translateY = new QDoubleSpinBox;
    QDoubleSpinBox * m_translateZ = new QDoubleSpinBox;

    QSpinBox * m_rotateX = new QSpinBox;
    QSpinBox * m_rotateY = new QSpinBox;
    QSpinBox * m_rotateZ = new QSpinBox;

    QDoubleSpinBox * m_scaleX = new QDoubleSpinBox;
    QDoubleSpinBox * m_scaleY = new QDoubleSpinBox;
    QDoubleSpinBox * m_scaleZ = new QDoubleSpinBox;

    ngl::Vec3 m_pos;
    ngl::Vec3 m_rot;
    ngl::Vec3 m_scale;

    std::shared_ptr<SceneObject> m_curObject;

    bool update = true;
};

#endif