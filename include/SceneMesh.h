#ifndef MESH_H_
#define MESH_H_

#include <ngl/AbstractMesh.h>
#include "SceneObject.h"

//based on NGL's AbstractMesh and Obj classes, added support for triangulating faces while loading
class SceneMesh : public ngl::AbstractMesh, public SceneObject
{
  public:
    SceneMesh() noexcept: ngl::AbstractMesh(){};
    bool load(const std::string &_fname,CalcBB _calcBB=CalcBB::True) noexcept;
    void draw() override {}
     ~SceneMesh() {}
    
  private:


};

#endif