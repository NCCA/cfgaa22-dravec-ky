#version 410 core
layout (location=0) in vec3 inPos;
layout (location=1) in vec3 inN;
layout (location=2) in vec2 inUV;

out vec2 TexCoords;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 inTransform;
uniform mat4 SpriteVP;

void main()
{
    WorldPos = vec3(inTransform * vec4(inPos,1.0f));
    gl_Position = SpriteVP*vec4(WorldPos,1.0);
    Normal = normalize(mat3(inTransform)*inN);
    TexCoords = inUV;
}
