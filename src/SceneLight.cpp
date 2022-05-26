#include "SceneLight.h"
#include "SceneManager.h"
#include "Utils.h"
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>

SceneLight::SceneLight(int _id)
{
    std::cout << "\nLight created!\n";
    m_id = _id;
    m_type = ObjectType::LIGHT;
}

void SceneLight::draw()
{
    transform.setScale(0.3,0.3,0.3);
    auto matrix = transform.getMatrix();
    ngl::ShaderLib::use("Unlit");
    SceneManager::loadCameraMatrixToCurrentShader();
    ngl::ShaderLib::setUniformMatrix4fv("inTransform",&matrix.m_00);
    auto col = ngl::Vec3(1);
    ngl::ShaderLib::setUniform("inCol",col);
    ngl::VAOPrimitives::draw("sphere");
    glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    ngl::ShaderLib::use("PBR");
}