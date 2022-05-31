#include "NGLSceneMenu.h"
#include "SceneObject.h"
#include "Utils.h"
#include "SceneManager.h"

#include <QWidget>
#include <QGroupBox>
#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QLabel>
#include <QColorDialog>
#include <QDateTime>
#include <QComboBox>
#include <ngl/ShaderLib.h>

NGLSceneMenu::NGLSceneMenu(NGLScene * _scene, QWidget *_parent)
{
    m_scene = _scene;
    setupInfoMenu();
    setupRenderMenu();
    setupShadowMenu();
    m_mainLayout->addSpacerItem(new QSpacerItem(10,10,QSizePolicy::Minimum,QSizePolicy::Expanding));
    this->setLayout(m_mainLayout);
}

void NGLSceneMenu::addToCounter()
{
    static long long prevTime = QDateTime::currentMSecsSinceEpoch();
    auto curTime = QDateTime::currentMSecsSinceEpoch();
    m_fpsCounter->setText(QString(fmt::format("FPS: {:.2f}",1000.0f/(curTime-prevTime)).c_str() ));
    prevTime = curTime;
}

void NGLSceneMenu::setupRenderMenu()
{   
    //Connection is set in MainWindow.cpp
    

    auto render_layout = new QGridLayout();
    auto render_box = new QGroupBox();

    std::vector<std::string> radio_names = {"Forward","Deferred","Wireframe","Albedo","Normal","PBR Maps","Shadow Maps","Emissive"};
    std::vector<void (NGLScene::*)()> radio_funcs = {   &NGLScene::renderForward, 
                                                        &NGLScene::renderDeferred,
                                                        &NGLScene::renderWireframe,
                                                        &NGLScene::renderDeferredAlbedo,
                                                        &NGLScene::renderDeferredNormal,
                                                        &NGLScene::renderDeferredORM,
                                                        &NGLScene::renderShadowMap,
                                                        &NGLScene::renderDeferredEmissive};
    std::vector<QRadioButton*> radio_widgets;
    std::vector<QLabel*> radio_labels;
    for(int i=0;i<radio_names.size();++i)
    {
        radio_labels.push_back(new QLabel());
        radio_labels[i]->setText(QString(radio_names[i].c_str()));
        radio_widgets.push_back(new QRadioButton());
        connect(radio_widgets[i], &QPushButton::clicked, [radio_funcs,i, this]{ SceneManager::setRenderFunction(radio_funcs[i]);});
        if(i<2) radio_labels[i]->setFont(QFont(radio_labels[i]->font().family(), 0,87));
    }

    render_layout->addWidget(radio_labels[0],0,1);
    render_layout->addWidget(radio_labels[1],0,3);
    render_layout->addWidget(radio_widgets[0],0,0);
    render_layout->addWidget(radio_widgets[1],0,2);
    radio_widgets[0]->setChecked(true);

    //separator
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    render_layout->addWidget(line,1,0,1,4);

    render_layout->addWidget(radio_labels[2],2,1);
    render_layout->addWidget(radio_labels[3],2,3);
    render_layout->addWidget(radio_labels[4],3,1);
    render_layout->addWidget(radio_labels[5],3,3);
    render_layout->addWidget(radio_labels[6],4,3);
    render_layout->addWidget(radio_labels[7],4,1);

    render_layout->addWidget(radio_widgets[2],2,0);
    render_layout->addWidget(radio_widgets[3],2,2);
    render_layout->addWidget(radio_widgets[4],3,0);
    render_layout->addWidget(radio_widgets[5],3,2);
    render_layout->addWidget(radio_widgets[6],4,2);
    render_layout->addWidget(radio_widgets[7],4,0);

    render_box->setLayout(render_layout);
    render_layout->setSizeConstraint(QLayout::SizeConstraint::SetMinAndMaxSize);
    render_layout->setColumnStretch(1,0);
    render_layout->setColumnStretch(2,0);
    
    m_mainLayout->addWidget(render_box);
    this->setLayout(m_mainLayout);
}

void NGLSceneMenu::setupShadowMenu()
{
    auto layout = new QGridLayout();
    auto box = new QGroupBox();

    auto label = new QLabel();
    QComboBox * int_box = new QComboBox();
    layout->addWidget(label, 0,0);
    layout->addWidget(int_box,0,1);

    label->setText("Shadow Resolution: ");
    int shadow_data [] = {256,512,1024,2048,4096,8192};
    
    for(int shadow_res : shadow_data)
    {
        int_box->addItem(QString(std::to_string(shadow_res).c_str() ), QVariant(shadow_res));
    }
    int_box->setCurrentIndex(2);
    connect(int_box, QOverload<int>::of(&QComboBox::currentIndexChanged),
                    [=](int idx){   m_scene->m_params.SHADOW_HEIGHT = int_box->currentData().toUInt();
                                    m_scene->m_params.SHADOW_WIDTH = int_box->currentData().toUInt();
                                    m_scene->updateOmniShadowSize(); } );
    

    label = new QLabel();
    label->setText("Shadow Filter Size: ");
    auto spinbox = new QDoubleSpinBox();
    layout->addWidget(label, 1,0);
    layout->addWidget(spinbox,1,1);
    spinbox->setValue(m_scene->m_params.SHADOW_OFFSET);
    spinbox->setSingleStep(0.01);
    connect(spinbox, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [=](double val){ 
                                    m_scene->m_params.SHADOW_OFFSET = val;
                                    m_scene->loadShaderDefaults("PBR");
                                    m_scene->loadShaderDefaults("DeferredPBR");});





    box->setLayout(layout);
    layout->setSizeConstraint(QLayout::SizeConstraint::SetMinAndMaxSize);
    m_mainLayout->addWidget(box);
    this->setLayout(m_mainLayout);
}

void NGLSceneMenu::setupInfoMenu()
{
    auto layout = new QGridLayout();
    auto box = new QGroupBox();

    m_fpsCounter->setText(QString("Testing..."));
    layout->addWidget(m_fpsCounter);

    //separator
    QFrame* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line,1,0,1,1);

    m_camPos->setText(QString("default"));
    layout->addWidget(m_camPos, 2,0);

    
    box->setLayout(layout);
    layout->setSizeConstraint(QLayout::SizeConstraint::SetMaximumSize);

    m_mainLayout->addWidget(box);
    this->setLayout(m_mainLayout);
}

// void NGLSceneMenu::setupShadowMenu()
// {
//     auto layout = new QGridLayout();
//     auto box = new QGroupBox();



//     box->setLayout(render_layout);
//     layout->setSizeConstraint(QLayout::SizeConstraint::SetFixedSize);
//     m_mainLayout->addWidget(box);
//     this->setLayout(m_mainLayout);
// }