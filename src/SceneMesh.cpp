#include "SceneMesh.h"
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
    m_obj = std::make_unique<MeshInfo>("../meshes/testgeo.obj");
    
    if(m_obj->isLoaded())
    {
        m_obj->createVAO();
        m_vao = m_obj->moveVAO();
        std::cout << std::endl << m_obj->getNumFaces();
        //std::cout << std::endl << m_vao->getBufferID();
        //std::cout << std::endl << m_obj->getBBox().width();
        VAO_loaded = true;
        isObj = true;
        
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
}

void SceneMesh::draw()
{
    if(VAO_loaded)
    {
        auto matrix = transform.getMatrix();
        ngl::ShaderLib::setUniformMatrix4fv("inTransform",&matrix.m_00);
        
        m_vao->bind();
        glBindBuffer(GL_ARRAY_BUFFER, m_vao->getBufferID());
        //std::cout << std::endl << m_vao->numIndices();
        if(isSelected)
        {
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
            m_vao->draw();
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        }
        else m_vao->draw();

        m_vao->unbind();
    }
}
SceneMesh::~SceneMesh()
{
    m_vao.release();
    std::cout << "Scene Mesh deleted!";
    
}
