#version 410 core
layout (location=0) in vec3 inPos;
layout (location=1) in vec3 inN;
layout (location=2) in vec2 inUV;
layout (location=3) in vec3 inTangent;
layout (location=4) in vec3 inBiTangent;

uniform mat4 inTransform;

out vec2 TexCoords;
out vec4 WorldPos;
out vec3 Normal;
out vec4 FragPosLightSpace;

out mat3 TBN;

uniform mat4 lightSpaceMatrix;

layout (std140) uniform TransformVP
{
    mat4 V;
    mat4 P;
    mat4 VP;
} t;

void main()
{
    vec3 T = normalize(vec3(inTransform * vec4(inTangent,   0.0)));
    vec3 B = normalize(vec3(inTransform * vec4(inBiTangent, 0.0)));
    vec3 N = normalize(vec3(inTransform * vec4(inN,    0.0)));
    TBN = mat3(T, B, N);

    WorldPos = inTransform * vec4(inPos,1.0f);
    Normal = normalize(mat3(inTransform)*inN);
    TexCoords = inUV;
    FragPosLightSpace = lightSpaceMatrix * vec4(WorldPos);

    gl_Position = t.VP*vec4(WorldPos);
}