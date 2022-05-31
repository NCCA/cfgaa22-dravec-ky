#ifndef MESH_H_
#define MESH_H_

#include <ngl/AbstractMesh.h>
#include <ngl/AbstractVAO.h>
#include "SceneObject.h"
#include <ngl/Obj.h>
#include <QImage>


class MeshInfo: public ngl::Obj
{
  public:
    MeshInfo( const std::string& _fname, CalcBB _calcBB=CalcBB::True)  noexcept: ngl::Obj(_fname, _calcBB){};
    ngl::AbstractVAO * getRawVAO() {return m_vaoMesh.get();};
    ~MeshInfo(){};
};

//based on NGL's AbstractMesh and Obj classes, added support for triangulating faces while loading
class SceneMesh : public SceneObject
{
  public:
    SceneMesh(const std::string &_name, const std::string &_fname);
    SceneMesh(const std::string &_primname);
    //bool load(const std::string &_fname,CalcBB _calcBB=CalcBB::True) noexcept;
    void draw(const std::string &_shaderName = "PBR") override;
    void loadMaterialToShader();
    void loadTransformToShader();
    void setWireframe();

     void setMaterialInfo(const Material &_mat) override ;
     Material getMaterial(){return m_material;};

    /// @brief Loads texture for the material
    /// 0 - Albedo
    /// 1 - OcclusionRoughnessMetallic
    /// 2 - Normal
    void loadTexture(int _index, const std::string &_path) override;

    /// @brief simple struct for hlding Material information. 
    /// Please change texture using SceneMesh::setTexture 
    /// if you don't want issues..
    Material m_material;
     ~SceneMesh();  
  protected:
    
    void load(const std::string &path);
    void initMaterial();
    void createVAO();

    /// @brief Sets texture for the material
    /// 0 - Albedo
    /// 1 - OcclusionRoughnessMetallic
    /// 2 - Normal
    void setTexture(int _index, const GLuint &_tex_id);

    std::unique_ptr<ngl::AbstractVAO> m_vao;
    bool VAO_loaded = false;

    bool isObj;
    std::unique_ptr<ngl::Obj> m_obj;

};

#endif