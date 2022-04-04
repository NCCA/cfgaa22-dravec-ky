#version 410 core

layout (location=0) out vec4 fragColour;

uniform vec4 ourColor;

void main()
{
    fragColour=ourColor;
}