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

    int i=0;
    for(QDoubleSpinBox * box : {m_translateX,m_translateY,m_translateZ} )
    {
        vbox->addWidget(box,1,i);
        box->setRange(-1000.0,1000.0);
        box->setSingleStep(0.1);
        connect(box, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val){ m_pos[i] = val; setTransforms();});
        i++;
    }

    QLabel *label_r = new QLabel();
    label_r->setText("Rotate");
    vbox->addWidget(label_r,2,0);

    i=0;
    for(QSpinBox * box : {m_rotateX,m_rotateY,m_rotateZ} )
    {
        vbox->addWidget(box,3,i);
        box->setRange(-1000,1000);
        box->setSingleStep(10);
        connect(box, QOverload<int>::of(&QSpinBox::valueChanged), [=](double val){ m_rot[i] = val; setTransforms();});
        i++;
    }

    QLabel *label_s = new QLabel();
    label_s->setText("Scale");
    vbox->addWidget(label_s,4,0);

    i=0;
    for(QDoubleSpinBox * box : {m_scaleX,m_scaleY,m_scaleZ} )
    {
        vbox->addWidget(box,5,i);
        box->setRange(-1000.0,1000.0);
        box->setSingleStep(0.1);
        connect(this->m_scaleX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val){ m_scale[i] = val; setTransforms();});
        i++;
    }

    this->setLayout(vbox);
}

void NGLObjectMenu::setTransforms()
{
    if(m_curObject && update)
    {
        m_curObject->transform.setPosition(m_pos);
        m_curObject->transform.setRotation(m_rot);
        m_curObject->transform.setScale(m_scale);
        SceneManager::update();
        if(m_curObject->getType()==SceneObject::ObjectType::LIGHT)
        {
            std::cout << "light update\n";
            SceneManager::updateLightInfo();
        }
            
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