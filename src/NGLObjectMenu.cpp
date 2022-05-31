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
#include <QFileDialog>

NGLObjectMenu::NGLObjectMenu(QWidget *_parent)
{
    
    setupLightMenu();  
    setupMaterialMenu();
    setupTransformMenu();

    m_mainLayout->addWidget(m_mainSplitter);
    m_mainSplitter->setOrientation(Qt::Orientation::Vertical);

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
        connect(box, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val){ m_scale[i] = val; setObject();});
        i++;
    }

    m_transformBox->setLayout(vbox);
    m_mainSplitter->addWidget(m_transformBox);
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

    //https://forum.qt.io/topic/88140/how-to-apply-colors-to-a-box-subwindow-within-mainwindow/8
    QLabel *label_c = new QLabel();
    label_c->setText("Light Colour: ");
    vbox->addWidget(label_c,1,0);

    vbox->addWidget(m_lightColour,1,1);
    connect(this->m_lightColour, &QPushButton::clicked, [this]{ m_l_col = getColourMenu(m_lightColour);setObject();});
    
    m_lightBox->setLayout(vbox);
    m_mainSplitter->addWidget(m_lightBox);

}

ngl::Vec3 NGLObjectMenu::getColourMenu(QWidget *w )
{   
    if( !w ) return ngl::Vec3(1.0f);;
    // auto dlg = new QColorDialog();
    // std::cout << "\n\n" << dlg->frameGeometry().x() << "   " << dlg->frameGeometry().y();
    // dlg->exec();
    // auto color = dlg->selectedColor();
    // std::cout << "\n\n" << dlg->frameGeometry().x() << "   " << dlg->frameGeometry().y() << std::endl;
    auto prevCol = w->palette().color(QPalette::Window);
    QColor color = QColorDialog::getColor(prevCol, w, QString(), QColorDialog::DontUseNativeDialog);
    if( color.isValid() )
    {  
        w->setStyleSheet( m_col_sheet.arg(color.name()) );
    }
    qreal r,g,b;
    color.getRgbF(&r,&g,&b);
    std::cout << fmt::format("{} {} {}",r,g,b);
    return ngl::Vec3(r,g,b);
}

std::string NGLObjectMenu::loadImage(QAbstractButton *w)
{
    auto path = QFileDialog::getOpenFileName(this, tr("Open Image File"), 
                                            "/", 
                                            tr("Image Files (*.png *.jpg *.bmp)"),(QString *)nullptr, QFileDialog::DontUseNativeDialog);  
    if(path==NULL || path.toStdString() == "")
    {
        std::cout << "File empty.";
        w->setIcon( QPixmap::fromImage(QImage()) );
        
        return "";
    }
    QImage img(path);
    w->setIcon( QPixmap::fromImage(img) );
    return path.toStdString();
}

void NGLObjectMenu::setupMaterialMenu()
{
    for(int i=0;i<4;++i)
    {
        m_textures.push_back(new QPushButton(" "));
        m_texture_paths.push_back("");
    }

    QGridLayout *vbox = new QGridLayout;

    QLabel *label_t1 = new QLabel();
    label_t1->setText("Albedo Texture");
    vbox->addWidget(label_t1,0,1);
    QLabel *label_t2 = new QLabel();
    label_t2->setText("Occlusion/Roughness/Metallic Texture");
    vbox->addWidget(label_t2,1,1);
    QLabel *label_t3 = new QLabel();
    label_t3->setText("Normal Texture");
    vbox->addWidget(label_t3,2,1);
    QLabel *label_t4 = new QLabel();
    label_t4->setText("Emissive Texture");
    vbox->addWidget(label_t4,3,1);

    connect(this->m_textures[0], &QPushButton::clicked, 
        [this]{ m_texture_paths[0] = loadImage(m_textures[0]) ;setObject();});
    connect(this->m_textures[1], &QPushButton::clicked, 
        [this]{ m_texture_paths[1] = loadImage(m_textures[1]) ;setObject();});
    connect(this->m_textures[2], &QPushButton::clicked, 
        [this]{ m_texture_paths[2] = loadImage(m_textures[2]) ;setObject();});
    connect(this->m_textures[3], &QPushButton::clicked, 
        [this]{ m_texture_paths[3] = loadImage(m_textures[3]) ;setObject();});
    
    for(int i=0;i<4;++i)
    {
        vbox->addWidget(m_textures[i],i,0);
        m_textures[i]->setFixedSize(QSize(50,50));
        m_textures[i]->setIconSize(QSize(50,50));
    }
    
    QLabel *label_c = new QLabel();
    label_c->setText("Albedo: ");
    vbox->addWidget(label_c,4,0);
    
    vbox->addWidget(m_Albedo,4,1);
    connect(this->m_Albedo, &QPushButton::clicked, [this]{ m_m_albedo = getColourMenu(m_Albedo);setObject();});
    
    QLabel *label_i = new QLabel();
    label_i->setText("Roughness: ");
    vbox->addWidget(label_i,5,0);

    vbox->addWidget(m_Roughness,5,1);
    connect(this->m_Roughness, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val){ m_m_rough = val;setObject();});
    m_Roughness->setRange(-1000.0,1000.0);
    m_Roughness->setSingleStep(0.1);

    QLabel *label_m = new QLabel();
    label_m->setText("Metallic: ");
    vbox->addWidget(label_m,6,0);

    vbox->addWidget(m_Metallic,6,1);
    connect(this->m_Metallic, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val){ m_m_metallic = val;setObject();});
    m_Metallic->setRange(-1000.0,1000.0);
    m_Metallic->setSingleStep(0.1);

    QLabel *label_a = new QLabel();
    label_a->setText("AO: ");
    vbox->addWidget(label_a,7,0);

    vbox->addWidget(m_AO,7,1);
    connect(this->m_AO, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val){ m_m_ao = val;setObject();});
    m_AO->setRange(-1000.0,1000.0);
    m_AO->setSingleStep(0.1);

    QLabel *label_e = new QLabel();
    label_e->setText("Emissive: ");
    vbox->addWidget(label_e,8,0);

    vbox->addWidget(m_Emissive,8,1);
    connect(this->m_Emissive, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val){ m_m_emissive = val;setObject();});
    m_Emissive->setRange(-1000.0,1000.0);
    m_Emissive->setSingleStep(0.1);

    QLabel *label_n = new QLabel();
    label_n->setText("Normal Blend: ");
    vbox->addWidget(label_n,9,0);

    vbox->addWidget(m_Normal,9,1);
    connect(this->m_Normal, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val){ m_m_normal = val;setObject();});
    m_Normal->setRange(-1000.0,1000.0);
    m_Normal->setSingleStep(0.1);

    m_materialBox->setLayout(vbox);
    m_mainSplitter->addWidget(m_materialBox);
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

                mesh->loadTexture(0,m_texture_paths[0]);
                mesh->loadTexture(1,m_texture_paths[1]);
                mesh->loadTexture(2,m_texture_paths[2]);
                mesh->loadTexture(3,m_texture_paths[3]);


                mesh->m_material.albedo = m_m_albedo;
                mesh->m_material.roughness = m_m_rough;
                mesh->m_material.metallic = m_m_metallic;
                mesh->m_material.ao = m_m_ao;
                mesh->m_material.emissive = m_m_emissive;
                mesh->m_material.normal = m_m_normal;
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
            
            QImage img;

            for(int i=0;i<4;++i)
            {
                m_texture_paths[i] = mesh->m_material.texture_paths[i];
                if(mesh->m_material.texture_icons[i].isNull())
                {
                    img = QImage(QString(m_texture_paths[i].c_str()));
                    mesh->m_material.texture_icons[i]=img;
                }
                else img = mesh->m_material.texture_icons[i];
                m_textures[i]->setIcon(QPixmap::fromImage(img));
                m_texture_paths[i] = mesh->m_material.texture_paths[i];
            }

            m_m_albedo = mesh->m_material.albedo;
            m_m_rough = mesh->m_material.roughness;
            m_m_metallic = mesh->m_material.metallic;
            m_m_ao = mesh->m_material.ao;
            m_m_emissive = mesh->m_material.emissive;
            m_m_normal = mesh->m_material.normal;

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