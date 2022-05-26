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
    enum class ObjectType {MESH, LIGHT, PRIMITIVE};

    SceneObject();
    virtual ~SceneObject();
    int childCount() const;
    static void setParentChild(std::shared_ptr<SceneObject> _parent, std::shared_ptr<SceneObject> _child);
    std::shared_ptr<SceneObject> getChild(int index);
    std::shared_ptr<SceneObject> getParent() {return m_parent;}
    bool removeChild(int index);
    ObjectType getType() {return m_type;};

    //OpenGL
    virtual void drawInherited();
    virtual void draw() {}

    std::string getName() {return m_name;}
    void setName(const std::string &_name) {m_name = _name;}

    int indexOf(std::shared_ptr<SceneObject> _child) const;

    ngl::Transformation transform;

    bool isSelected = false;
    
  protected:

    ObjectType m_type;
    
    void setParent(std::shared_ptr<SceneObject> _parent);
    void addChild(std::shared_ptr<SceneObject> _child);
    //void loadShaderInfo();

    std::vector<std::shared_ptr<SceneObject>> m_children;
    std::shared_ptr<SceneObject> m_parent;

    std::string m_name;
    int m_index;

};

#endif