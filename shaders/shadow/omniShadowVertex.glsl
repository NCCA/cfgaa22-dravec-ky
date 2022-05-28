#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 inTransform;

void main()
{
    gl_Position = inTransform * vec4(aPos, 1.0);
}  