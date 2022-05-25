#include "ScenePrimitive.h"
#include "ngl/ShaderLib.h"
#include "ngl/VAOPrimitives.h"


ScenePrimitive::ScenePrimitive(const std::string &_name)
{
    m_name = _name;
}

void ScenePrimitive::draw()
{   
    auto matrix = transform.getMatrix();
    ngl::ShaderLib::setUniformMatrix4fv("inTransform",&matrix.m_00);
    ngl::VAOPrimitives::draw(m_name);
}