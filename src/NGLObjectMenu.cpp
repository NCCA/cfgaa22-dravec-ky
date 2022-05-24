#include "NGLObjectMenu.h"
#include "SceneObject.h"
#include "Utils.h"
#include "SceneManager.h"

#include <QWidget>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QLabel>

NGLObjectMenu::NGLObjectMenu(QWidget *_parent)
{
    m_translateX->stepBy(0.1);

    QGridLayout *vbox = new QGridLayout;

    QLabel *label_t = new QLabel();
    label_t->setText("Translate");
    vbox->addWidget(label_t,0,0);

    vbox->addWidget(m_translateX,1,0);
    vbox->addWidget(m_translateY,1,1);
    vbox->addWidget(m_translateZ,1,2);

    QLabel *label_r = new QLabel();
    label_r->setText("Rotate");
    vbox->addWidget(label_r,2,0);

    vbox->addWidget(m_rotateX,3,0);
    vbox->addWidget(m_rotateY,3,1);
    vbox->addWidget(m_rotateZ,3,2);

    QLabel *label_s = new QLabel();
    label_s->setText("Scale");
    vbox->addWidget(label_s,4,0);

    vbox->addWidget(m_scaleX,5,0);
    vbox->addWidget(m_scaleY,5,1);
    vbox->addWidget(m_scaleZ,5,2);

    this->setLayout(vbox);

    connect(this->m_translateX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ m_pos[0] = i; setTransforms();});
    connect(this->m_translateY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ m_pos[1] = i; setTransforms();});
    connect(this->m_translateZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ m_pos[2] = i; setTransforms();});

    connect(this->m_rotateX, QOverload<int>::of(&QSpinBox::valueChanged), [=](double i){ m_rot[0] = i; setTransforms();});
    connect(this->m_rotateY, QOverload<int>::of(&QSpinBox::valueChanged), [=](double i){ m_rot[1] = i; setTransforms();});
    connect(this->m_rotateZ, QOverload<int>::of(&QSpinBox::valueChanged), [=](double i){ m_rot[2] = i; setTransforms();});

    connect(this->m_scaleX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ m_scale[0] = i; setTransforms();});
    connect(this->m_scaleY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ m_scale[1] = i; setTransforms();});
    connect(this->m_scaleZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double i){ m_scale[2] = i; setTransforms();});
}

void NGLObjectMenu::setTransforms()
{
    if(m_curObject && update)
    {
        m_curObject->transform.setPosition(m_pos);
        m_curObject->transform.setRotation(m_rot);
        m_curObject->transform.setScale(m_scale);
        SceneManager::update();
        //std::cout<<"\nupdated transforms.";
    }
}

void NGLObjectMenu::updateObject(std::shared_ptr<SceneObject> _obj)
{
    update = false;
    m_curObject = _obj;
    //std::cout<<"\nupdated object";

    m_pos = _obj->transform.getPosition();
    m_rot = _obj->transform.getRotation();
    m_scale = _obj->transform.getScale();

    m_translateX->setValue(m_pos[0]);
    m_translateY->setValue(m_pos[1]);
    m_translateZ->setValue(m_pos[2]);

    m_rotateX->setValue(m_rot[0]);
    m_rotateY->setValue(m_rot[1]);
    m_rotateZ->setValue(m_rot[2]);

    m_scaleX->setValue(m_scale[0]);
    m_scaleY->setValue(m_scale[1]);
    m_scaleZ->setValue(m_scale[2]);
    SceneManager::update();
    update = true;

}