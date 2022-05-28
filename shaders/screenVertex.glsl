
#version 330 core
//https://learnopengl.com/Advanced-OpenGL/Framebuffers
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0); 
    TexCoords = inTexCoords;
}  