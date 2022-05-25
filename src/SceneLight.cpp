#include "SceneLight.h"
#include "Utils.h"

SceneLight::SceneLight(ngl::Vec3 _pos, int _intensity, ngl::Vec3 _col)
{
 std::cout << "\nLight created!\n";
 this->transform.setPosition(_pos);
 m_intensity = _intensity;
 m_colour = _col;
}

void SceneLight::draw()
{

}