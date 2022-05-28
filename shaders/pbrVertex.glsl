#version 410 core
layout (location=0) in vec3 inPos;
layout (location=1) in vec3 inN;
layout (location=2) in vec2 inUV;

uniform mat4 inTransform;

out vec2 TexCoords;
out vec4 WorldPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

uniform mat4 lightSpaceMatrix;

layout (std140) uniform TransformVP
{
    mat4 V;
    mat4 P;
    mat4 VP;
} t;

void main()
{
    WorldPos = inTransform * vec4(inPos,1.0f);
    Normal = normalize(mat3(inTransform)*inN);
    TexCoords = inUV;
    FragPosLightSpace = lightSpaceMatrix * vec4(WorldPos);

    gl_Position = t.VP*vec4(WorldPos);
}