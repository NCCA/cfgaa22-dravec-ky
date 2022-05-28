#version 330 core
layout (location = 0) out vec4 m_gBufferPosition;
layout (location = 1) out vec4 m_gBufferNormal;
layout (location = 2) out vec4 m_gBufferAlbedo;
layout (location = 3) out vec4 m_gBufferAORoughnessMetallic;

in vec2 TexCoords;
in vec4 WorldPos;
in vec3 Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_ao_rough_met1;

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

//https://learnopengl.com/Advanced-Lighting/Deferred-Shading

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    m_gBufferPosition = vec4(WorldPos);//WorldPos;
    // also store the per-fragment normals into the gbuffer
    m_gBufferNormal = vec4(Normal,1);//normalize(Normal);
    // and the diffuse per-fragment color
    m_gBufferAlbedo = vec4(albedo,1);//texture(texture_diffuse1, TexCoords).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    m_gBufferAORoughnessMetallic = vec4(ao,roughness,metallic,1);//texture(texture_ao_rough_met1, TexCoords).rgb;
}  