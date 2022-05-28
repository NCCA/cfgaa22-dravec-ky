#version 330 core
//https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
layout (location = 0) in vec3 inPos;

uniform mat4 lightSpaceMatrix;
uniform mat4 inTransform;

void main()
{
    gl_Position = lightSpaceMatrix * inTransform * vec4(inPos, 1.0);
}  