#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "SceneManager.h"
#include <fmt/format.h>

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent), m_ui(new Ui::MainWindow)
{
  m_ui->setupUi(this);

  m_gl= new  NGLScene(this);
  m_list = new NGLSceneTreeView(this);

  SceneManager::initialize(m_gl, m_list);

  m_ui->s_mainWindowGridLayout->addWidget(m_gl,0,0,3,1);

  connect(m_ui->m_wireframe,SIGNAL(toggled(bool)),m_gl,SLOT(toggleWireframe(bool)));
  // set the rotation signals
  connect(m_ui->m_rotationX,SIGNAL(va()lueChanged(double)),m_gl,SLOT(setXRotation(double)));
  connect(m_ui->m_rotationY,SIGNAL(valueChanged(double)),m_gl,SLOT(setYRotation(double)));
  connect(m_ui->m_rotationZ,SIGNAL(valueChanged(double)),m_gl,SLOT(setZRotation(double)));
  /// set the scale signals
  connect(m_ui->m_scaleX,SIGNAL(valueChanged(double)),m_gl,SLOT(setXScale(double)));
  connect(m_ui->m_scaleY,SIGNAL(valueChanged(double)),m_gl,SLOT(setYScale(double)));
  connect(m_ui->m_scaleZ,SIGNAL(valueChanged(double)),m_gl,SLOT(setZScale(double)));
  /// set the position signals
  connect(m_ui->m_positionX,SIGNAL(valueChanged(double)),m_gl,SLOT(setXPosition(double)));
  connect(m_ui->m_positionY,SIGNAL(valueChanged(double)),m_gl,SLOT(setYPosition(double)));
  connect(m_ui->m_positionZ,SIGNAL(valueChanged(double)),m_gl,SLOT(setZPosition(double)));
  /// set the combo box index change signal
  connect(m_ui->m_objectSelection,SIGNAL(currentIndexChanged(int)),m_gl,SLOT(this->onClick()));
  connect(m_ui->m_colour,SIGNAL(clicked()),m_gl,SLOT(setColour()));
  // set load button

  connect(m_ui->actionLoad, &QAction::triggered, this, [this]{ loadObjectFromFile(); });
  connect(m_ui->actionSphere, &QAction::triggered, this, [this]{ loadObject("sphere"); });
  connect(m_ui->actionTeapot, &QAction::triggered, this, [this]{ loadObject("teapot"); });
  connect(m_ui->actionCube, &QAction::triggered, this, [this]{ loadObject("cube"); });
  
  
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