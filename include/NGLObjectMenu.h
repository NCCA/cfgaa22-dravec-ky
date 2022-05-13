#ifndef NGLOBJECTMENU_H_
#define NGLOBJECTMENU_H_

#include <QGroupBox>

//base class definition from https://doc.qt.io/archives/4.6/model-view-creating-models.html
class NGLObjectMenu : QGroupBox
{
    Q_OBJECT
  public:
    NGLObjectMenu(QWidget *parent = nullptr);
    ~NGLObjectMenu() {}
//     NGLObjectMenu(QWidget *parent = nullptr);
//     ~NGLObjectMenu() {}
//     std::shared_ptr<SceneObject> getSceneRoot() const;

//   public slots:
//     void add(std::shared_ptr<SceneObject> obj);
//     void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    

//   private:
//     QStringList m_stringList;
//     SceneTreeModel * m_treeModel;

//     // QModelIndexList selectedIndexes() const override;
//     // void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;
//     void keyPressEvent(QKeyEvent *event) override;
//     void mousePressEvent(QMouseEvent *event) override;

};

#endif