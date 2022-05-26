#version 430 core
// this demo is based on code from here https://learnopengl.com/#!PBR/Lighting
/// @brief the vertex passed in
layout (location = 0) in vec3 inVert;
/// @brief the normal passed in
layout (location = 1) in vec3 inNormal;
/// @brief the in uv
layout (location = 2) in vec2 inUV;

layout (location=3) uniform vec3 inPos;

layout (location=4) uniform vec3 inScale;

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
  //worldPos = vec3(inTransform * vec4(inVert, 1.0f));
  //normal=normalize(mat3(transforms.normalMatrix)*inNormal);
  vertCol = vec3(1.0,1.0,1.0);

  gl_Position = transforms.MVP*vec4(inPos,0.0f)+vec4(inVert[0]*inScale[0],inVert[2]*inScale[2],0.0f,0.0f);
}
