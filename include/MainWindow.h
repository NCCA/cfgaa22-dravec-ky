#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "NGLScene.h"
#include "NGLSceneTreeView.h"
#include "NGLObjectMenu.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *m_ui;
    void loadObjectFromFile();
    void loadObject(const std::string &_value);

public slots:

    void setTranslateX(double _val);
    void updateMenu();
    // void setTranslateY(double _val);
    // void setTranslateZ(double _val);

    // void setRotateX(double _val);
    // void setRotateY(double _val);
    // void setRotateZ(double _val);

    // void setScaleX(double _val);
    // void setScaleY(double _val);
    // void setScaleZ(double _val);

    // void setVisibility(bool _val);

private:
        
		/// @brief our openGL widget
		NGLScene *m_gl;
        NGLSceneTreeView *m_list;
        NGLObjectMenu *m_menu;
};


#endif // MAINWINDOW_H
