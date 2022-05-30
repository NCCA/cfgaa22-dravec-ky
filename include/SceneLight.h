#ifndef SCENE_LIGHT_H_
#define SCENE_LIGHT_H_

#include "SceneObject.h"

class SceneLight : public SceneObject
{
    public:
      SceneLight(int _id);
      void draw(const std::string &_shaderName = "PBR") override;
      void setIntensity(int _intensity) {m_intensity = _intensity;};
      void setColour(ngl::Vec3 _col) {m_colour = _col;};

      int getID() {return m_id;};
      ngl::Vec3 getColour() {return m_colour;};
      float getIntensity() {return m_intensity;};

      ~SceneLight() {};
    private:
        float m_intensity = 1.0f;
        ngl::Vec3 m_colour = ngl::Vec3(1.0f,1.0f,1.0f);
        int m_id = -1;
        int m_info_id;
};
#endif
