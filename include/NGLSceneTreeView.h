#ifndef NGLSCENELISTVIEW_H_
#define NGLSCENELISTVIEW_H_

#include <QTreeView>
#include <QStringListModel>
#include <QAbstractItemModel>
#include "SceneObject.h"
#include "SceneTreeModel.h"

//base class definition from https://doc.qt.io/archives/4.6/model-view-creating-models.html
class NGLSceneTreeView: public QTreeView
{
    Q_OBJECT
  public:
    NGLSceneTreeView(QWidget *parent = nullptr);
    ~NGLSceneTreeView() {}
    std::shared_ptr<SceneObject> getSceneRoot();
    bool removeSelected();
    std::shared_ptr<SceneObject> getSelectedObject();

  public slots:
    void add(std::shared_ptr<SceneObject> obj);
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    
  signals:
    void selectionChangedSignal();

  private:
    QStringList m_stringList;
    SceneTreeModel * m_treeModel;

    bool isLoaded = false;

    // QModelIndexList selectedIndexes() const override;
    // void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    
    

};

#endif