#ifndef SCENE_LIGHT_H_
#define SCENE_LIGHT_H_

#include "SceneObject.h"

class SceneLight : public SceneObject
{
    public:
      SceneLight(int _id);
      void draw() override;
      void setIntensity(int _intensity) {m_intensity = _intensity;};
      void setColour(ngl::Vec3 _col) {m_colour = _col;};
      ~SceneLight() {};
    private:
        int m_intensity = 1;
        ngl::Vec3 m_colour = ngl::Vec3(1.0f,1.0f,1.0f);
        int m_id = -1;
};
#endif