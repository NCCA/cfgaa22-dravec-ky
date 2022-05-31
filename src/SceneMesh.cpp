#include "SceneMesh.h"
#include "SceneManager.h"

#include <iostream>
#include <QFileDialog>
#include <ngl/Obj.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Obj.h>
#include <ngl/SimpleVAO.h>
#include <ngl/ShaderLib.h>
#include "SceneManager.h"
#include <ngl/VAOFactory.h>



SceneMesh::SceneMesh(const std::string &_name, const std::string &_fname)
{
    m_name = _name;
    m_path = _fname;
    isObj = true;
    m_type = ObjectType::MESH;
    initMaterial();
}

void SceneMesh::load(const std::string &path)
{
    auto obj = std::make_unique<ngl::Obj>(path);
    m_obj = std::move(obj);
    
    if(m_obj->isLoaded())
    {
        createVAO();
        //m_vao = m_obj->moveVAO();
        std::cout<<"\n\nLoaded vertices: " << m_vao->numIndices();
        VAO_loaded = true;
    }
}

SceneMesh::SceneMesh(const std::string &_primname)
{
    m_name = _primname;

    ngl::SimpleVAO * raw_vao = reinterpret_cast<ngl::SimpleVAO *>(ngl::VAOPrimitives::getVAOFromName(_primname));

    if(raw_vao!=nullptr)
    {
        m_vao.reset(raw_vao);
        VAO_loaded = true;
    }
    else if(raw_vao == nullptr) std::cout << m_name <<"Invalid primitive VAO\n";
    m_type = ObjectType::PRIMITIVE;
    initMaterial();
}

void SceneMesh::initMaterial()
{
    for(int i=0;i<4;++i)
    {
        m_material.textures.push_back(SceneManager::getDefaultTexture(i));
        m_material.texture_paths.push_back("");
        m_material.texture_icons.push_back(QImage());
    }
}

void SceneMesh::loadTexture(int _index, const std::string &_path)
{
    //std::cout << "\nTrying to replace " << m_material.texture_paths[_index] << " with " << _path;
    if(_path!="" && _path != m_material.texture_paths[_index])
    {   
        ngl::Texture tex = ngl::Texture();
        std::cout <<std::endl<< m_name << " loading texture " << _path << std::endl << std::endl;
        bool success = tex.loadImage(_path);
        if(success)
        {
            
            setTexture(_index, tex.setTextureGL());
            // auto data = tex.getPixels();
            // GLuint tex_id;
            // glGenTextures(1, &tex_id);
            // glBindTexture(GL_TEXTURE_2D, tex_id);
            // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex.getWidth(), tex.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            // setTexture(_index, tex_id);
            m_material.texture_paths[_index]=_path;
        }
    }
    else if (_path == "")
    {
        
        setTexture(_index,SceneManager::getDefaultTexture(_index));
        m_material.texture_paths[_index]=_path;
        m_material.texture_icons[_index] = QImage();
    }
}

void SceneMesh::setMaterialInfo(const Material &mat)
{
    m_material.albedo = mat.albedo;
    m_material.metallic = mat.metallic;
    m_material.roughness = mat.roughness;
    m_material.ao = mat.ao;
    m_material.emissive = mat.emissive;
    m_material.normal = mat.normal;
}

void SceneMesh::loadMaterialToShader()
{
    int isPrimitive = m_type==SceneObject::ObjectType::PRIMITIVE ? 1 : 0;
    if(m_material.texture_paths[2]=="" || isPrimitive) m_material.normal = 0;

    ngl::ShaderLib::setUniform("inAlbedo",    m_material.albedo.m_r,
                                            m_material.albedo.m_g,
                                            m_material.albedo.m_b);
    ngl::ShaderLib::setUniform("inMetallic",  m_material.metallic);
    ngl::ShaderLib::setUniform("inRoughness", m_material.roughness);
    ngl::ShaderLib::setUniform("inAO",        m_material.ao);
    ngl::ShaderLib::setUniform("inEmissive",        m_material.emissive);
    ngl::ShaderLib::setUniform("inNormalMult", 
            m_material.normal);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_material.textures[0]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_material.textures[1]);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_material.textures[2]);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_material.textures[3]);
}

void SceneMesh::setTexture(int _i, const GLuint &_tex_id)
{
    if(m_material.texture_paths[_i]!= "") glDeleteTextures(1, &m_material.textures[_i]);
   m_material.textures[_i] = _tex_id;
}

void SceneMesh::loadTransformToShader()
{
    auto matrix = transform.getMatrix();
    ngl::ShaderLib::setUniformMatrix4fv("inTransform",&matrix.m_00);
}

void SceneMesh::setWireframe()
{
    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
    glPolygonOffset(-0.2,0.0);
    ngl::ShaderLib::use("Unlit");

    SceneManager::loadCameraMatrixToCurrentShader();
    auto col = ngl::Vec3(1);
    ngl::ShaderLib::setUniform("inCol",ngl::Vec4(col,0.1f));

    auto matrix = transform.getMatrix();
    ngl::ShaderLib::setUniformMatrix4fv("inTransform",&matrix.m_00);

}

void SceneMesh::draw(const std::string &_shaderName)
{   
    
    if(isObj && !VAO_loaded)
    {
        load(m_path);
    }
    if(VAO_loaded)
    {   
        m_vao->bind();
        loadTransformToShader();
        SceneManager::loadCameraMatrixToCurrentShader();

        if(_shaderName=="PBR")
        {
            loadMaterialToShader();
            if(isSelected)
            {
                setWireframe();
                m_vao->draw();
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                ngl::ShaderLib::use("PBR");
            }
        }

        if(_shaderName == "gBufferPBR")
        {
            loadMaterialToShader();
        }

        m_vao->draw();
        m_vao->unbind();
    }
}

struct vertData
{
	GLfloat u; // tex cords from obj
	GLfloat v; // tex cords
	GLfloat nx; // normal from obj mesh
	GLfloat ny;
	GLfloat nz;
	GLfloat x; // position from obj
	GLfloat y;
	GLfloat z;
	GLfloat tx; // tangent calculated by us
	GLfloat ty;
	GLfloat tz;
	GLfloat bx; // binormal (bi-tangent really) calculated by us
	GLfloat by;
	GLfloat bz;
};

//https://github.com/NCCA/NGL6Demos/blob/master/NormalMapping/src/NGLScene.cpp
void SceneMesh::createVAO()
{
  std::vector <ngl::Vec3> verts=m_obj->getVertexList();
  std::vector <ngl::Face> faces=m_obj->getFaceList();
  std::vector <ngl::Vec3> tex=m_obj->getUVList();
  std::vector <ngl::Vec3> normals=m_obj->getNormalList();

	//std::cout<<"got mesh data\n";
	std::vector <vertData> vboMesh;
	vertData d;
	unsigned int nFaces=faces.size();
	unsigned int nNorm=normals.size();
	unsigned int nTex=tex.size();
    //std::cout << "\n faces: " << nFaces;
	// loop for each of the faces
	for(unsigned int i=0;i<nFaces;++i)
	{
		// now for each triangle in the face (remember we ensured tri above)
		for(int j=0;j<3;++j)
		{
			// pack in the vertex data first
			d.x=verts[faces[i].m_vert[j]].m_x;
			d.y=verts[faces[i].m_vert[j]].m_y;
			d.z=verts[faces[i].m_vert[j]].m_z;
			// now if we have norms of tex (possibly could not) pack them as well
			if(nNorm >0 && nTex > 0)
			{
				d.nx=normals[faces[i].m_norm[j]].m_x;
				d.ny=normals[faces[i].m_norm[j]].m_y;
				d.nz=normals[faces[i].m_norm[j]].m_z;

				d.u=tex[faces[i].m_uv[j]].m_x;
				d.v=tex[faces[i].m_uv[j]].m_y;

      }
      // now if neither are present (only verts like Zbrush models)
      else if(nNorm ==0 && nTex==0)
      {
        d.nx=0;
        d.ny=0;
        d.nz=0;
        d.u=0;
        d.v=0;
      }
      // here we've got norms but not tex
      else if(nNorm >0 && nTex==0)
      {
        d.nx=normals[faces[i].m_norm[j]].m_x;
        d.ny=normals[faces[i].m_norm[j]].m_y;
        d.nz=normals[faces[i].m_norm[j]].m_z;
        d.u=0;
        d.v=0;

      }
      // here we've got tex but not norm least common
      else if(nNorm ==0 && nTex>0)
      {
        d.nx=0;
        d.ny=0;
        d.nz=0;
        d.u=tex[faces[i].m_uv[j]].m_x;
        d.v=tex[faces[i].m_uv[j]].m_y;
      }
    // now we calculate the tangent / bi-normal (tangent) based on the article here
    // http://www.terathon.com/code/tangent.html

    ngl::Vec3 c1 = normals[faces[i].m_norm[j]].cross(ngl::Vec3(0.0, 0.0, 1.0));
    ngl::Vec3 c2 = normals[faces[i].m_norm[j]].cross(ngl::Vec3(0.0, 1.0, 0.0));
    ngl::Vec3 tangent;
    ngl::Vec3 binormal;
    if(c1.length()>c2.length())
    {
      tangent = c1;
    }
    else
    {
      tangent = c2;
    }
    // now we normalize the tangent so we don't need to do it in the shader
    tangent.normalize();
    // now we calculate the binormal using the model normal and tangent (cross)
    binormal = normals[faces[i].m_norm[j]].cross(tangent);
    // normalize again so we don't need to in the shader
    binormal.normalize();
    d.tx=tangent.m_x;
    d.ty=tangent.m_y;
    d.tz=tangent.m_z;
    d.bx=binormal.m_x;
    d.by=binormal.m_y;
    d.bz=binormal.m_z;
    vboMesh.push_back(d);
    }
  }
	//m_vao = ngl::VAOFactory::createVAO("simpleVAO");
    //std::cout << "\nFinished tangent calculation.";
	m_vao->bind();
	unsigned int meshSize=vboMesh.size();
	// now we have our data add it to the VAO, we need to tell the VAO the following
	// how much (in bytes) data we are copying
	// a pointer to the first element of data (in this case the address of the first element of the
	// std::vector
    std::vector<GLfloat> meshData;
    for(vertData &vert : vboMesh)
    {
        GLfloat vertData[] = {  vert.x,vert.y,vert.z,
                                vert.u, vert.v, 
                                vert.nx, vert.ny, vert.nz,
                                vert.tx, vert.ty, vert.tz,
                                vert.bx, vert.by, vert.bz};
        for(GLfloat val : vertData)
            meshData.push_back(val);
    }
    //std::cout << "\nSorted vertices to GLfloat vector.";
	auto data = ngl::AbstractVAO::VertexData(meshData.size()*sizeof(GLfloat),meshData[0]);
    //inPos
    m_vao->setData(data);
	m_vao->setVertexAttributePointer(0,3,GL_FLOAT,sizeof(GLfloat)*14,0);
    //std::cout<<"\npos";
    //inN
	m_vao->setVertexAttributePointer(1,3,GL_FLOAT,sizeof(GLfloat)*14,5);
    //std::cout<<"\nn";
    //inUV
	m_vao->setVertexAttributePointer(2,2,GL_FLOAT,sizeof(GLfloat)*14,3);
    //std::cout<<"\nuv";
    //Tangent
	m_vao->setVertexAttributePointer(3,3,GL_FLOAT,sizeof(GLfloat)*14,8);
    //std::cout<<"\nt";
    //Bi-Tangent
    m_vao->setVertexAttributePointer(4,3,GL_FLOAT,sizeof(GLfloat)*14,11);
    std::cout << "\nCreated vao with " << meshSize << " vertices.";
    
	m_vao->setNumIndices(meshSize);
    std::cout<<m_vao->numIndices();
	m_vao->unbind();
}

SceneMesh::~SceneMesh()
{
    m_vao.release();
    for(int i=0;i<m_material.texture_paths.size();++i)
        if(m_material.texture_paths[i]!= "") glDeleteTextures(1, &m_material.textures[i]);
    std::cout << "Scene Mesh deleted!";
    
}
