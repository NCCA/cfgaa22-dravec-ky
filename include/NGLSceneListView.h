#ifndef NGLSCENELISTVIEW_H_
#define NGLSCENELISTVIEW_H_

#include <QListView>
#include <QStringListModel>
#include "SceneObject.h"


//base class definition from https://doc.qt.io/archives/4.6/model-view-creating-models.html
class NGLSceneListView: public QListView
{
    Q_OBJECT
  public:
    NGLSceneListView(QWidget *parent = nullptr);
    ~NGLSceneListView() {}

  public slots:
    void push_back(const std::string &_name = "lol");

  private:
    QStringList m_stringList;
    QStringListModel * m_listModel;
};

#endif