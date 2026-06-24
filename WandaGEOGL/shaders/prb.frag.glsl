// shaders/pbr.frag — PBR complet 
#version 330 core 
 
uniform sampler2D u_albedo; 
uniform sampler2D u_normal; 
uniform sampler2D u_metallic; 
uniform sampler2D u_roughness; 
uniform sampler2D u_ao; 
 
// Distribution GGX (Trowbridge-Reitz) 
float D_GGX(float NdotH, float roughness) { 
    float a  = roughness * roughness; 
    float a2 = a * a; 
    float d  = (NdotH*NdotH * (a2 - 1.0) + 1.0); 
    return a2 / (3.14159 * d * d); 
} 
 
// Géométrie Smith-GGX 
float G_Smith(float NdotV, float NdotL, float roughness) { 
    float r  = roughness + 1.0; 
    float k  = (r*r) / 8.0; 
    float gv = NdotV / (NdotV * (1-k) + k); 
    float gl = NdotL / (NdotL * (1-k) + k); 
    return gv * gl; 
} 
 
// Fresnel Schlick 
vec3 F_Schlick(float cosTheta, vec3 F0) { 
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0); 
} 