#ifndef NGLOBJECTMENU_H_
#define NGLOBJECTMENU_H_

#include <QGroupBox>
#include <QSpinBox>
#include "SceneObject.h"
#include <QLayout>
#include <QBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QSplitter>

//base class definition from https://doc.qt.io/archives/4.6/model-view-creating-models.html
class NGLObjectMenu : public QGroupBox
{
    Q_OBJECT
  public:
    NGLObjectMenu(QWidget *parent = nullptr);
    ~NGLObjectMenu() {};
    void updateObject(std::shared_ptr<SceneObject> _obj);
    void setObject();
    void setTranslate(int _i, double _val);
    void setRotate(int _i, double _val);
    void setScale(int _i, double _val);
    ngl::Vec3 getColourMenu(QWidget *w);
    
  public slots:

  private:
    void setupTransformMenu();
    void setupMaterialMenu();
    void setupLightMenu();
    std::string loadImage(QAbstractButton *w);

    QBoxLayout * m_mainLayout = new QBoxLayout(QBoxLayout::Direction::TopToBottom);
    QSplitter * m_mainSplitter = new QSplitter();

    QGroupBox * m_transformBox = new QGroupBox;
    QDoubleSpinBox * m_translateX = new QDoubleSpinBox;
    QDoubleSpinBox * m_translateY = new QDoubleSpinBox;
    QDoubleSpinBox * m_translateZ = new QDoubleSpinBox;
    QSpinBox * m_rotateX = new QSpinBox;
    QSpinBox * m_rotateY = new QSpinBox;
    QSpinBox * m_rotateZ = new QSpinBox;
    QDoubleSpinBox * m_scaleX = new QDoubleSpinBox;
    QDoubleSpinBox * m_scaleY = new QDoubleSpinBox;
    QDoubleSpinBox * m_scaleZ = new QDoubleSpinBox;
    ngl::Vec3 m_pos;
    ngl::Vec3 m_rot;
    ngl::Vec3 m_scale;

    QGroupBox * m_lightBox = new QGroupBox;
    QDoubleSpinBox * m_lightIntensity = new QDoubleSpinBox;
    QPushButton * m_lightColour = new QPushButton("   ");
    float m_l_int;
    ngl::Vec3 m_l_col;

    QGroupBox * m_materialBox = new QGroupBox;
    QToolButton * m_texAlbedo = new QToolButton();
    QPushButton * m_texORM = new QPushButton("   ");
    QPushButton * m_texNormal = new QPushButton("   ");
    QPushButton * m_texEmissive = new QPushButton("   ");
    std::string m_path_albedo = "";
    std::string m_path_orm = "";
    std::string m_path_normal = "";
    std::string m_path_emissive = "";

    std::vector<QPushButton*> m_textures;
    std::vector<std::string> m_texture_paths;
    
    QPushButton * m_Albedo = new QPushButton("   ");
    QDoubleSpinBox * m_Roughness = new QDoubleSpinBox;
    QDoubleSpinBox * m_Metallic = new QDoubleSpinBox;
    QDoubleSpinBox * m_AO = new QDoubleSpinBox;
    QDoubleSpinBox * m_Emissive = new QDoubleSpinBox;
    QDoubleSpinBox * m_Normal = new QDoubleSpinBox;
    ngl::Vec3 m_m_albedo;
    float m_m_rough;
    float m_m_metallic;
    float m_m_ao;
    float m_m_emissive;
    float m_m_normal;

    std::shared_ptr<SceneObject> m_curObject;

    bool update = true;

    QString m_col_sheet = R"(
    QPushButton {
    background-color: %1;
    }
    )";
};

#endif