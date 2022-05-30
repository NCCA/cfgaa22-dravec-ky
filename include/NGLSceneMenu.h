#ifndef NGLSCENEMENU_H_
#define NGLSCENEMENU_H_

#include <QGroupBox>
#include <QSpinBox>
#include "SceneObject.h"
#include <QLayout>
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>

//base class definition from https://doc.qt.io/archives/4.6/model-view-creating-models.html
class NGLSceneMenu : public QGroupBox
{
    Q_OBJECT
  public:
    NGLSceneMenu(QWidget *parent = nullptr);
    ~NGLSceneMenu() {};
    QLabel * m_fpsCounter = new QLabel();
    void addToCounter();
  public slots:

  private:
    void setupRenderMenu();


    QBoxLayout * m_mainLayout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);


    ngl::Vec3 m_m_albedo;
    float m_m_rough;
    float m_m_metallic;
    float m_m_ao;

    std::shared_ptr<SceneObject> m_curObject;

    bool update = true;

    QString m_col_sheet = R"(
    QPushButton {
    background-color: %1;
    }
    )";
};

#endif