#version 410 core

layout (location=0) out vec4 fragColour;

in vec3 vertCol;
in vec2 UV;
in vec3 worldPos;

uniform sampler2D outTex;

void main()
{
    fragColour=vec4(vertCol,1.0f);
}