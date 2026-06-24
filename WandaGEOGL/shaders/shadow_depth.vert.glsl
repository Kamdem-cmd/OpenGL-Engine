
// Étape 1 : rendu depuis la lumière (depth pass) 
// shaders/shadow_depth.vert 
#version 330 core 
layout(location=0) in vec3 aPos; 
uniform mat4 u_lightSpaceMVP; 
void main() { gl_Position = u_lightSpaceMVP * vec4(aPos, 1.0); } 