#version 410 core

layout (location=0) out vec4 fragColour;

in vec3 vertCol;
in vec2 UV;
in vec3 worldPos;

uniform vec3 col;
uniform float diff;
uniform float spec;

uniform sampler2D outTex;

struct Material
{
  vec3 diffuse;
  vec3 specular;
  vec3 ambient;
  float shininess;
};

uniform Material material;

void main()
{
    fragColour=vec4(vertCol,1.0f);
}