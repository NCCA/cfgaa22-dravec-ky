#include "NGLObjectMenu.h"
#include "SceneObject.h"
#include "Utils.h"
#include "SceneManager.h"

#include <QWidget>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QColorDialog>

NGLObjectMenu::NGLObjectMenu(QWidget *_parent)
{
    setupTransformMenu();
    setupLightMenu();  
    setupMaterialMenu();
    
    this->setLayout(m_mainLayout);
}

void NGLObjectMenu::setupTransformMenu()
{
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
        connect(box, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val){ m_pos[i] = val; setObject();});
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
        connect(box, QOverload<int>::of(&QSpinBox::valueChanged), [=](double val){ m_rot[i] = val; setObject();});
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
        connect(this->m_scaleX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val){ m_scale[i] = val; setObject();});
        i++;
    }

    m_transformBox->setLayout(vbox);
    m_mainLayout->addWidget(m_transformBox);
}

void NGLObjectMenu::setupLightMenu()
{
    QGridLayout *vbox = new QGridLayout;

    QLabel *label_i = new QLabel();
    label_i->setText("Light Intensity: ");
    vbox->addWidget(label_i,0,0);

    vbox->addWidget(m_lightIntensity,0,1);
    connect(this->m_lightIntensity, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val){ m_l_int = val;setObject();});
    m_lightIntensity->setRange(-1000.0,1000.0);
    m_lightIntensity->setSingleStep(0.1);
    // connect(box, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val){ m_pos[i] = val; setObject();});
    // i++;

    //https://forum.qt.io/topic/88140/how-to-apply-colors-to-a-box-subwindow-within-mainwindow/8
    QLabel *label_c = new QLabel();
    label_c->setText("Light Colour: ");
    vbox->addWidget(label_c,1,0);

    vbox->addWidget(m_lightColour,1,1);
    connect(this->m_lightColour, &QPushButton::clicked, [this]{ m_l_col = getColourMenu(m_lightColour);setObject();});
    
    m_lightBox->setLayout(vbox);
    m_mainLayout->addWidget(m_lightBox);

}

ngl::Vec3 NGLObjectMenu::getColourMenu(QWidget *w )
{   
    
    if( !w ) return ngl::Vec3(1.0f);;
    QColor color = QColorDialog::getColor(Qt::yellow);
    if( color.isValid() )
    {  
        w->setStyleSheet( m_col_sheet.arg(color.name()) );
    }
    qreal r,g,b;
    color.getRgbF(&r,&g,&b);
    std::cout << fmt::format("{} {} {}",r,g,b);
    return ngl::Vec3(r,g,b);
}

void NGLObjectMenu::setupMaterialMenu()
{
    QGridLayout *vbox = new QGridLayout;
    
    QLabel *label_c = new QLabel();
    label_c->setText("Albedo: ");
    vbox->addWidget(label_c,0,0);

    vbox->addWidget(m_Albedo,0,1);
    connect(this->m_Albedo, &QPushButton::clicked, [this]{ m_m_albedo = getColourMenu(m_Albedo);setObject();});
    
    QLabel *label_i = new QLabel();
    label_i->setText("Roughness: ");
    vbox->addWidget(label_i,1,0);

    vbox->addWidget(m_Roughness,1,1);
    connect(this->m_Roughness, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val){ m_m_rough = val;setObject();});
    m_Roughness->setRange(-1000.0,1000.0);
    m_Roughness->setSingleStep(0.1);

    QLabel *label_m = new QLabel();
    label_m->setText("Metallic: ");
    vbox->addWidget(label_m,2,0);

    vbox->addWidget(m_Metallic,2,1);
    connect(this->m_Metallic, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val){ m_m_metallic = val;setObject();});
    m_Metallic->setRange(-1000.0,1000.0);
    m_Metallic->setSingleStep(0.1);

    QLabel *label_a = new QLabel();
    label_a->setText("AO: ");
    vbox->addWidget(label_a,3,0);

    vbox->addWidget(m_AO,3,1);
    connect(this->m_AO, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val){ m_m_ao = val;setObject();});
    m_AO->setRange(-1000.0,1000.0);
    m_AO->setSingleStep(0.1);

    m_materialBox->setLayout(vbox);
    m_mainLayout->addWidget(m_materialBox);
}

void NGLObjectMenu::setObject()
{
    if(m_curObject && update)
    {
        switch(m_curObject->getType())
        {
            case SceneObject::ObjectType::LIGHT:
            {
                auto light = std::static_pointer_cast<SceneLight>(m_curObject);
                light->setIntensity(m_l_int);
                light->setColour(m_l_col);
                SceneManager::updateLightInfo();
                break;
            }
            default:
            {
                auto mesh = std::static_pointer_cast<SceneMesh>(m_curObject);
                mesh->m_material.albedo = m_m_albedo;
                mesh->m_material.roughness = m_m_rough;
                mesh->m_material.metallic = m_m_metallic;
                mesh->m_material.ao = m_m_ao;
                break;
            }
        }
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
    switch(m_curObject->getType())
    {
        case SceneObject::ObjectType::LIGHT:
        {
            m_lightBox->setVisible(true);
            m_materialBox->setVisible(false);

            auto light = std::static_pointer_cast<SceneLight>(m_curObject);
            m_l_int = light->getIntensity();
            m_l_col = light->getColour();

            m_lightIntensity->setValue(m_l_int);
            auto col = QColor(  static_cast<int>(m_l_col.m_r*255.0),
                                static_cast<int>(m_l_col.m_g*255.0),
                                static_cast<int>(m_l_col.m_b*255.0));
            m_lightColour->setStyleSheet( m_col_sheet.arg(col.name()) );
            break;
        }
        default:
        {
            m_lightBox->setVisible(false);
            m_materialBox->setVisible(true);

            auto mesh = std::static_pointer_cast<SceneMesh>(m_curObject);
            m_m_albedo = mesh->m_material.albedo;
            m_m_rough = mesh->m_material.roughness;
            m_m_metallic = mesh->m_material.metallic;
            m_m_ao = mesh->m_material.ao;

            auto col = QColor(  static_cast<int>(m_m_albedo.m_r*255.0),
                                static_cast<int>(m_m_albedo.m_g*255.0),
                                static_cast<int>(m_m_albedo.m_b*255.0));
            m_Albedo->setStyleSheet( m_col_sheet.arg(col.name()) );
            m_Roughness->setValue(m_m_rough);
            m_Metallic->setValue(m_m_metallic);
            m_AO->setValue(m_m_ao);
            break;
        }
    }

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