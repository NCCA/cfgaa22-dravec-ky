#version 410 core

layout (location=0) out vec4 fragColour;
in vec4 outColor;
in vec2 outUV;

uniform sampler2D outTex;

void main()
{
    fragColour=texture(outTex, outUV);
}