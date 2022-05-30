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

NGLSceneMenu::NGLSceneMenu(QWidget *_parent)
{
    setupRenderMenu();
    
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
    QGridLayout *vbox = new QGridLayout;

    auto m_r_buttonGroup = new QButtonGroup();

    auto m_box = new QGroupBox();

    m_fpsCounter->setText(QString("Testing..."));

    vbox->addWidget(m_fpsCounter);
    m_box->setLayout(vbox);

    auto m_r_wireframe = new QRadioButton();
    auto m_r_normal = new QRadioButton();
    auto m_r_deferred = new QRadioButton();

    m_mainLayout->addWidget(m_box);
    this->setLayout(m_mainLayout);

}
