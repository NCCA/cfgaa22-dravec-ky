#include "SceneMesh.h"
#include "SceneManager.h"

#include <iostream>
#include <QFileDialog>
#include <ngl/Obj.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Obj.h>
#include <ngl/SimpleVAO.h>
#include <ngl/ShaderLib.h>



SceneMesh::SceneMesh(const std::string &_name, const std::string &_fname)
{
    m_name = _name;
    m_path = _fname;
    isObj = true;
    m_type = ObjectType::MESH;
    std::cout << "\nScene Mesh Created!";
}

void SceneMesh::load(const std::string &path)
{
    auto obj = std::make_unique<ngl::Obj>(path);
    m_obj = std::move(obj);
    
    if(m_obj->isLoaded())
    {
        m_obj->createVAO();
        
        m_vao = m_obj->moveVAO();
        VAO_loaded = true;
    }
}

SceneMesh::SceneMesh(const std::string &_primname)
{
    m_name = _primname;

    ngl::SimpleVAO * raw_vao = reinterpret_cast<ngl::SimpleVAO *>(ngl::VAOPrimitives::getVAOFromName(_primname));

    if(raw_vao!=nullptr)
    {
        std::cout << "penis\n";
        m_vao.reset(raw_vao);
        VAO_loaded = true;
    }
    else if(raw_vao == nullptr) std::cout << m_name <<"\n\nnull ptr vao lol";
    m_type = ObjectType::PRIMITIVE;
    draw();
}

void SceneMesh::draw()
{   
    ngl::ShaderLib::use("PBR");
    if(isObj && !VAO_loaded)
    {
        load(m_path);
    }
    if(VAO_loaded)
    {
        auto matrix = transform.getMatrix();
        ngl::ShaderLib::setUniformMatrix4fv("inTransform",&matrix.m_00);
        m_vao->bind();
        if(isSelected)
        {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            glPolygonOffset(-0.1,0.0);
            ngl::ShaderLib::use("Unlit");
            
            SceneManager::loadCameraMatrixToCurrentShader();
            ngl::ShaderLib::setUniformMatrix4fv("inTransform",&matrix.m_00);
            auto col = ngl::Vec3(1);
            ngl::ShaderLib::setUniform("inCol",col);

            m_vao->draw();

            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
            ngl::ShaderLib::use("PBR");
        }
        m_vao->draw();
        

        m_vao->unbind();
    }
}

SceneMesh::~SceneMesh()
{
    m_vao.release();
    std::cout << "Scene Mesh deleted!";
    
}
