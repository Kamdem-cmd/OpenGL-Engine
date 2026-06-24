// Fragment shader texturé 
// shaders/textured.frag 
#version 330 core 
in  vec2 vUV; 
out vec4 FragColor; 
uniform sampler2D u_albedo; 
uniform sampler2D u_normalMap; 
 
void main() { 
    vec4 color = texture(u_albedo, vUV); 
    FragColor  = color; 
}