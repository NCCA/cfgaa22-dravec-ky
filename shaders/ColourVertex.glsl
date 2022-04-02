#version 410 core
layout (location=0) in vec3 inPosition;

uniform mat4 MVP;

void main()
{
    //gl_Position=MVP*vec4(inPosition,1.0);
    gl_Position = vec4(inPosition.x,inPosition.y,inPosition.z,1.0);
}