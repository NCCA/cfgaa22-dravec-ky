#version 430 core
layout (location=0) in vec3 inPos;
layout (location=1) in vec3 inN;
layout (location=2) in vec2 inUV;
layout (location=3) uniform mat4 inTransform;

out vec3 vertCol;
out vec2 UV;
out vec3 worldPos;

layout( std140) uniform TransformUBO
{
  mat4 MVP;
  mat4 normalMatrix;
  mat4 M;
}transforms;

void main()
{
    worldPos = vec3(inTransform * vec4(inPos,1.0f));
    gl_Position = transforms.MVP*vec4(worldPos,1.0);
    vertCol = normalize(mat3(transforms.normalMatrix)*inN);
    UV = inUV;
}