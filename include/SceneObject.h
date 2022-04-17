#ifndef SCENE_OBJECT_H_
#define SCENE_OBJECT_H_

#include <string>
#include <ngl/Transformation.h>
#include <QAbstractItemModel>

//structure based on https://learnopengl.com/Guest-Articles/2021/Scene/Scene-Graph
//and https://doc.qt.io/qt-5/qtwidgets-itemviews-editabletreemodel-example.html
class SceneObject
{
  public:
    SceneObject();
    virtual ~SceneObject();

    int childCount() const;
    static void setParentChild(std::shared_ptr<SceneObject> _parent, std::shared_ptr<SceneObject> _child);
    std::shared_ptr<SceneObject> getChild(int index);
    std::shared_ptr<SceneObject> getParent() {return m_parent;}
    bool removeChild(int index);

    //OpenGL
    virtual void drawInherited();
    virtual void draw() {}
    std::string getName() {return m_name;}
    void setName(const std::string &_name) {m_name = _name;}
    //deprecated
    int getIndex() {return m_index;}
    void setIndex(int _index) {m_index = _index;}

    //QT
    QVariant data() const;
    bool setData(const QVariant &value);
    
    int columnCount() const {return 1;}
    bool insertColumns(int position, int columns) {return false;}
    bool removeColumns(int position, int columns) {return false;}

    int indexOf(std::shared_ptr<SceneObject> _child) const;

    ngl::Transformation transform;
    
  protected:

    void setParent(std::shared_ptr<SceneObject> _parent);
    void addChild(std::shared_ptr<SceneObject> _child);

    

    std::vector<std::shared_ptr<SceneObject>> m_children;
    std::shared_ptr<SceneObject> m_parent;

    std::string m_name;
    int m_index;

    //QT
    QVector<QVariant> q_data;
};

#endif