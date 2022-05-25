#ifndef SCENE_LIGHT_H_
#define SCENE_LIGHT_H_

#include "SceneObject.h"

class SceneLight : public SceneObject
{
    public:
      SceneLight(ngl::Vec3 _pos, int _intensity, ngl::Vec3 _col);
      void draw() override;
      ~SceneLight() {};
    private:
        int m_intensity;
        ngl::Vec3 m_colour;
};
#endif