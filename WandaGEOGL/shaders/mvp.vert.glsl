// shaders/mvp.vert 
#version 460 core 
layout(location=0) in vec3 aPos; 
layout(location=1) in vec3 aNormal; 
layout(location=2) in vec2 aUV; 
 
uniform mat4 u_model; 
uniform mat4 u_view; 
uniform mat4 u_proj; 
 
out vec3 vNormal; 
out vec3 vFragPos; 
out vec2 vUV; 
 
void main() { 
    vec4 worldPos  = u_model * vec4(aPos, 1.0); 
    gl_Position    = u_proj * u_view * worldPos; 
    vFragPos       = worldPos.xyz; 
    vNormal        = mat3(transpose(inverse(u_model))) * aNormal; 
    vUV            = aUV; 
} 