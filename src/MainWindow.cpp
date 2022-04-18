#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "SceneManager.h"
#include <fmt/format.h>
#include <string>

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent), m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);

  m_gl= new  NGLScene(this);
  m_list = new NGLSceneTreeView(this);
  m_menu = new NGLObjectMenu(this);

  SceneManager::initialize(m_gl, m_list);

  m_ui->s_mainWindowSplitter->insertWidget(0, m_gl);
  m_ui->s_menuSplitter->insertWidget(0, m_list);

  //m_ui->s_listGridLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
  //m_ui->s_listGridLayout->SetFixedSize()
  

  //connect(m_ui->m_wireframe,SIGNAL(toggled(bool)),m_gl,SLOT(toggleWireframe(bool)));
  // set the rotation signals
  // connect(m_ui->m_rotationX,SIGNAL(valueChanged(double)),m_gl,SLOT(setXRotation(double)));
  // connect(m_ui->m_rotationY,SIGNAL(valueChanged(double)),m_gl,SLOT(setYRotation(double)));
  // connect(m_ui->m_rotationZ,SIGNAL(valueChanged(double)),m_gl,SLOT(setZRotation(double)));
  // /// set the scale signals
  // connect(m_ui->m_scaleX,SIGNAL(valueChanged(double)),m_gl,SLOT(setXScale(double)));
  // connect(m_ui->m_scaleY,SIGNAL(valueChanged(double)),m_gl,SLOT(setYScale(double)));
  // connect(m_ui->m_scaleZ,SIGNAL(valueChanged(double)),m_gl,SLOT(setZScale(double)));
  // /// set the position signals
  // connect(m_ui->m_positionX,SIGNAL(valueChanged(double)),m_gl,SLOT(setXPosition(double)));
  // connect(m_ui->m_positionY,SIGNAL(valueChanged(double)),m_gl,SLOT(setYPosition(double)));
  // connect(m_ui->m_positionZ,SIGNAL(valueChanged(double)),m_gl,SLOT(setZPosition(double)));
  /// set the combo box index change signal
  // connect(m_ui->m_objectSelection,SIGNAL(currentIndexChanged(int)),m_gl,SLOT(this->onClick()));
  // connect(m_ui->m_colour,SIGNAL(clicked()),m_gl,SLOT(setColour()));
  // set load button

  connect(m_ui->actionLoad, &QAction::triggered, this, [this]{ loadObjectFromFile(); });
  connect(m_ui->actionSphere, &QAction::triggered, this, [this]{ loadObject("cube"); });
  connect(m_ui->actionTeapot, &QAction::triggered, this, [this]{ loadObject("teapot"); });
  connect(m_ui->actionCube, &QAction::triggered, this, [this]{ loadObject("cube"); });
  
  for(int i=0;i<10;i++)
  {
    SceneManager::addObject("", SceneManager::ObjectType::PRIMITIVE, "cube");
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
  auto pathList = path.split("/");
}

void MainWindow::loadObject(const std::string &_value)
{
  SceneManager::addObject("", SceneManager::ObjectType::PRIMITIVE, _value);

}