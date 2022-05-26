#version 430 core

layout (location=0) out vec4 fragColour;

in vec3 vertCol;
in vec2 UV;
in vec3 worldPos;

uniform vec3 inCol;

void main()
{
    fragColour=vec4(inCol, 1.0f);
}