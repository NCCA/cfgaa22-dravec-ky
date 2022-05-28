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

NGLSceneMenu::NGLSceneMenu(QWidget *_parent)
{
    setupRenderMenu();
    
    this->setLayout(m_mainLayout);
}

void NGLSceneMenu::setupRenderMenu()
{   
    QGridLayout *vbox = new QGridLayout;

    auto m_r_buttonGroup = new QButtonGroup();

    auto m_r_wireframe = new QRadioButton();
    auto m_r_normal = new QRadioButton();
    auto m_r_deferred = new QRadioButton();

}
