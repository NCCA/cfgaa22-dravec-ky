#ifndef SCENE_PRIMITIVE_H_
#define SCENE_PRIMITIVE_H_

#include "SceneObject.h"
#include "SceneMesh.h"

//Temporary wrapper for calling VAOPrimitives draw functions using the Scene Manager for testing
class ScenePrimitive : public SceneObject
{
  public:
    ScenePrimitive(const std::string &_name);
    void draw (const std::string &_shaderName = "PBR") override;
    ~ScenePrimitive() {};
protected:
};

#endif