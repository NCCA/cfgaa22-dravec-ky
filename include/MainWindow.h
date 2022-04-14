#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "NGLScene.h"
#include "NGLSceneTreeView.h"

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

private:
        
		/// @brief our openGL widget
		NGLScene *m_gl;
        NGLSceneTreeView *m_list;
};


#endif // MAINWINDOW_H
