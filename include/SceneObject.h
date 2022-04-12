#ifndef SCENE_OBJECT_H_
#define SCENE_OBJECT_H_

#include <string>

class SceneObject
{
  public:
    SceneObject()=default;
    virtual ~SceneObject() {}
    virtual void draw() {}
    std::string getName() {return m_name;}
    int getIndex() {return m_index;}
    void setIndex(int _index) {m_index = _index;}
  protected:
    std::string m_name;
    int m_index;
};

#endif