#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "SceneManager.h"
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

  // /// set the scale signals
  // connect(m_ui->m_scaleX,SIGNAL(valueChanged(double)),m_gl,SLOT(setXScale(double)));
  // connect(m_ui->m_scaleY,SIGNAL(valueChanged(double)),m_gl,SLOT(setYScale(double)));
  // connect(m_ui->m_scaleZ,SIGNAL(valueChanged(double)),m_gl,SLOT(setZScale(double)));
  // /// set the position signals
  // connect(m_ui->m_positionX,SIGNAL(valueChanged(double)),m_gl,SLOT(setXPosition(double)));
  // connect(m_ui->m_positionY,SIGNAL(valueChanged(double)),m_gl,SLOT(setYPosition(double)));
  // connect(m_ui->m_positionZ,SIGNAL(valueChanged(double)),m_gl,SLOT(setZPosition(double)));
  /// set the combo box index change signal
  // connect(m_ui->m_objectSelection,SIGNAL(currentIndexChanged(int)),m_gl,SLOT(this->onClconnect(m_ui->actionLoad, &QAction::triggered, this, [this]{ loadObjectFromFile(); });
  // set load button

  connect(m_ui->actionLoad, &QAction::triggered, this, [this]{ loadObjectFromFile(); });
  connect(m_ui->actionSphere, &QAction::triggered, this, [this]{ loadObject("cube"); });
  connect(m_ui->actionTeapot, &QAction::triggered, this, [this]{ loadObject("teapot"); });
  connect(m_ui->actionCube, &QAction::triggered, this, [this]{ loadObject("cube"); });
  
  std::shared_ptr<SceneObject> obj;

  for(int i=0;i<10;i++)
  {
    obj = SceneManager::addObject("", SceneManager::ObjectType::PRIMITIVE, "teapot");
    obj->transform.setPosition(ngl::Vec3(i));
    SceneManager::update();
  }
  
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
  auto pathList = path.split("/");
}
void MainWindow::loadObject(const std::string &_value)
{
  SceneManager::addObject("", SceneManager::ObjectType::PRIMITIVE, _value);
}