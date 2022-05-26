#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "SceneManager.h"
#include "SceneObject.h"
#include <fmt/format.h>
#include <string>

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent), m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);

  m_gl = new  NGLScene(this);
  m_list = new NGLSceneTreeView(this);
  m_menu = new NGLObjectMenu(this);

  SceneManager::initialize(m_gl, m_list, m_menu);

  m_ui->s_mainWindowSplitter->insertWidget(0, m_gl);
  m_ui->s_menuSplitter->insertWidget(0, m_menu);
  m_ui->s_menuSplitter->insertWidget(0, m_list);


  //m_ui->s_listGridLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
  //m_ui->s_listGridLayout->SetFixedSize()
  // m_ui->m_translateX->setValue(8);
  // m_ui->m_translateY->setValue(5);
  // m_ui->m_translateZ->setValue(5);

  //connect(m_ui->m_wireframe,SIGNAL(toggled(bool)),m_gl,SLOT(toggleWireframe(bool)));
  // set the rotation signals
  // connect(m_ui->m_translateX,SIGNAL(valueChanged(double)),this,SLOT(setTranslateX(double)));
  // connect(m_ui->m_translateY,SIGNAL(valueChanged(double)),this,SLOT(setTranslateY(double)));
  // connect(m_ui->m_translateZ,SIGNAL(valueChanged(double)),this,SLOT(setTranslateZ(double)));
  
  connect(m_list,SIGNAL(selectionChangedSignal()),this,SLOT(updateMenu()));

  connect(m_ui->actionLoad, &QAction::triggered, this, [this]{ loadObjectFromFile(); });

  connect(m_ui->actionSphere, &QAction::triggered, this, [this]{ loadObject("sphere"); });
  connect(m_ui->actionTeapot, &QAction::triggered, this, [this]{ loadObject("teapot"); });
  connect(m_ui->actionPlane, &QAction::triggered, this, [this]{ loadObject("cube"); });
  connect(m_ui->actionCylinder, &QAction::triggered, this, [this]{ loadObject("cylinder"); });
  connect(m_ui->actionTorus, &QAction::triggered, this, [this]{ loadObject("torus"); });
  


  
}

void MainWindow::setTranslateX(double _val)
{
    auto selected = m_list->getSelectedObject();
    auto pos = selected->transform.getPosition();
    pos[0] = _val;
    selected->transform.setPosition(pos);
    SceneManager::update();
}

void MainWindow::updateMenu()
{
  auto selected = m_list->getSelectedObject();
  if(selected)
  {
    auto pos = selected->transform.getPosition();
    //m_ui->m_translateX->setValue(pos[0]);
  }
}

MainWindow::~MainWindow()
{ 
    delete m_ui;
}

void MainWindow::loadObjectFromFile()
{
  auto path = QFileDialog::getOpenFileName(this, tr("Open OBJ File"), 
                                            "/", 
                                            tr("Obj Files (*.obj)"));  
  if(path==NULL)
  {
    std::cout << "File empty.";
    return;
  }
  auto path_list = path.split("/");
  auto name = path_list.last().split(".").first();

  SceneManager::addObject(name.toStdString(), SceneObject::ObjectType::MESH, path.toStdString());
}

void MainWindow::loadObject(const std::string &_value)
{
  SceneManager::addObject("", SceneObject::ObjectType::PRIMITIVE, _value);
}