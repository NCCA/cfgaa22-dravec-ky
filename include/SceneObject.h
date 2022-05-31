#ifndef SCENE_OBJECT_H_
#define SCENE_OBJECT_H_

#include <string>
#include <ngl/Transformation.h>
#include <QAbstractItemModel>

struct Material
{
  std::vector<GLuint> textures;
  std::vector<std::string> texture_paths;
  std::vector<QImage> texture_icons;
  
  ngl::Vec3 albedo = ngl::Vec3(1.0f, 1.0f, 1.0f);
  float metallic = 1.0f;
  float roughness = 1.0f;
  float ao = 1.0f;
  float emissive = 1.0f;
  float normal = 1.0f;
};

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
    virtual void drawInherited(const std::string &_shaderName = "PBR");
    virtual void draw(const std::string &_shaderName = "PBR") {};
    virtual void loadTexture(int _index, const std::string &_path) {};
    std::string getName() {return m_name;};
    void setName(const std::string &_name) {m_name = _name;};

    virtual void setMaterialInfo(const Material &_mat) {};

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
    std::string m_path;
    int m_index;

    bool canDraw = true;

};

#endif