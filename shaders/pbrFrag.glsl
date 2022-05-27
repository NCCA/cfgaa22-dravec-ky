#version 330 core
//Based on https://learnopengl.com/PBR/Theory and NCCA PBR fragment shader
layout (location =0) out vec4 FragColour;

in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;
in vec4 FragPosLightSpace;

uniform vec3 camPos;

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

//uniform sampler2D shadowMap;
uniform samplerCube shadowCubeMap;

uniform float far_plane;
uniform vec3 lightPos;

uniform vec3 lightPositions[@numLights];
uniform vec3 lightColours[@numLights];
uniform float lightInts[@numLights];

uniform float exposure;

const float PI = 3.14159265359;
/*
0 - PBR
1 - normals;
2 - depth;
3 - diffuse?

*/
uniform int type;

//https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
float ShadowCalculationOmni(vec3 fragPos)
{
    vec3 fragToLight = fragPos - lightPos; 
    float closestDepth = texture(shadowCubeMap, fragToLight).r;
    
    closestDepth *= far_plane;
    
    float currentDepth = length(fragToLight);
    // now test for shadows
    float bias = 0.05; 
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    return shadow;

    
}
// //https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping
// float ShadowCalculationDir(vec4 fragPosLightSpace)
// {
//     float shadow = 0.0; 
//     // perform perspective divide - result in range (-1,1)
//     vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
//     // transform to [0,1] range
//     projCoords = projCoords * 0.5 + 0.5;

//     if(projCoords.z > 1.0)
//     {
//         return shadow;
//     }
//     // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
//     float closestDepth = texture(shadowMap, projCoords.xy).r; 
//     // get depth of current fragment from light's perspective
//     float currentDepth = projCoords.z;
//     // check whether current frag pos is in shadow
//     float bias = 0.005;
     

//     vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
//     for(int x = -1; x <= 1; ++x)
//     {
//         for(int y = -1; y <= 1; ++y)
//         {
//             float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
//             shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
//         }    
//     }
//     shadow /= 9.0;

//     return shadow;
// }

//primary function approximating microfacets - how many are aligned to the halfway vector
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}

//self-shadowing property of microfacets
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float nom   = NdotV;
    float denom = NdotV * (1.0 - roughness) + roughness;
	
    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

//fresnel
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{		
    vec3 N = Normal;
    vec3 V = normalize(camPos - WorldPos);

    //surface reflection at zero incidence
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    
    //Direct lighting
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < @numLights; ++i)
    {
        vec3 L = normalize(lightPositions[i] - WorldPos);
        vec3 H = normalize(V + L);

        float temp = max(dot(L,N),0.0);

        

        float distance = length(lightPositions[i] - WorldPos);
        float atten = (1.0 / (distance * distance))*lightInts[i];
        vec3 radiance = min(lightColours[i] * atten,1.0);

        //FragColour = vec4(radiance,1.0);

        vec3 F = fresnelSchlick(max(dot(H,V), 0.0), F0);
        float NDF = DistributionGGX(N,H, roughness);
        float G = GeometrySmith(N, V, L, roughness);


        vec3 numerator    = NDF * G * F;
        //adding epsilon so none end up 0
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0)  + 0.0001;
        vec3 specular     = numerator / denominator;  

        //kS - energy of light that gets reflected
        vec3 kS = F;
        //kD - light that gets refracted
        vec3 kD = vec3(1.0) - kS;
        //since metallic don't refract, we multiply
        kD *= 1.0 - metallic;

        float NdotL = max(dot(N,L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }
    float shadow = ShadowCalculationOmni(WorldPos);
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo*(1-shadow);

    //tonemapping HDR color using Reinhard operator
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

    FragColour = vec4(color, 1.0);  
}