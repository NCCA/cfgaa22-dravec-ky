#version 330 core
layout (location = 0) out vec4 m_gBufferAlbedo;
layout (location = 1) out vec4 m_gBufferAORoughnessMetallic;
layout (location = 2) out vec4 m_gBufferNormal;
layout (location = 3) out vec4 m_gBufferEmissive;

in vec2 TexCoords;
in vec4 WorldPos;
in vec3 Normal;

in mat3 TBN;

uniform samplerCube shadowCubeMap;
uniform sampler2D tex_Albedo;
uniform sampler2D tex_AORoughMet;
uniform sampler2D tex_Normal;
uniform sampler2D tex_Emissive;

uniform vec3 inAlbedo;
uniform float inMetallic;
uniform float inRoughness;
uniform float inAO;
uniform float inEmissive;

uniform int useNormalMap;

//https://learnopengl.com/Advanced-Lighting/Deferred-Shading

void main()
{  
    if(useNormalMap==1)
        m_gBufferNormal = vec4(normalize(TBN*(texture(tex_Normal, TexCoords).rgb*2.0-1.0)),1.0);
    else
        m_gBufferNormal = vec4(Normal,1.0);
    //m_gBufferNormal = vec4(normalize(TBN*(texture(tex_Normal, TexCoords).rgb*2.0-1.0)),1.0);
    //m_gBufferNormal = texture(tex_Normal, TexCoords);
    vec3 albedo = texture(tex_Albedo, TexCoords).rgb;
    albedo = albedo / (albedo + vec3(1.0));
    albedo = pow(albedo, vec3(1.0/2.2)) * inAlbedo;
    m_gBufferAlbedo = vec4(albedo,1);
    m_gBufferAORoughnessMetallic.b = texture(tex_AORoughMet, TexCoords).b * inMetallic + max(inMetallic-1,0);
    m_gBufferAORoughnessMetallic.g = texture(tex_AORoughMet, TexCoords).g * inRoughness + max(inRoughness-1,0);
    m_gBufferAORoughnessMetallic.r = texture(tex_AORoughMet, TexCoords).r * inAO + max(inAO-1,0);
    m_gBufferAORoughnessMetallic.a = 1.0f;
    vec3 emissive = texture(tex_Emissive, TexCoords).rgb*inEmissive + max(inEmissive-1,0);
    m_gBufferEmissive = vec4(emissive, 1.0);

    //gl_FragDepth = gl_FragCoord.z;
}  