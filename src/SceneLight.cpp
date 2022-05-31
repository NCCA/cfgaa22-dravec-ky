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

void SceneLight::draw(const std::string &_shaderName)
{
    if(_shaderName == "PBR" || _shaderName == "Unlit")
    {
        GLint prevMode;
        glGetIntegerv(GL_POLYGON_MODE, &prevMode);

        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        transform.setScale(0.5,0.5,0.5);
        auto matrix = transform.getMatrix();
        ngl::ShaderLib::use("Unlit");
        SceneManager::loadCameraMatrixToCurrentShader();

        //matrix = vp.VP;
        //ngl::ShaderLib::setUniformMatrix4fv("spriteVP",&matrix.m_00);

        ngl::ShaderLib::setUniformMatrix4fv("inTransform",&matrix.m_00);
        if(isSelected) ngl::ShaderLib::setUniform("inCol",ngl::Vec4(m_colour, 0.5));
        else ngl::ShaderLib::setUniform("inCol",ngl::Vec4(m_colour, 0.2));

        ngl::VAOPrimitives::draw("lightSphere");
        glPolygonMode( GL_FRONT_AND_BACK, prevMode );
        ngl::ShaderLib::use(_shaderName);
    }
    
}