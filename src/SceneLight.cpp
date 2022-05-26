#include "SceneLight.h"
#include "SceneManager.h"
#include "Utils.h"

SceneLight::SceneLight(int _id)
{
    std::cout << "\nLight created!\n";
    m_id = _id;
    m_type = ObjectType::LIGHT;
}

void SceneLight::draw()
{

}