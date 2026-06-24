
// shaders/phong.frag 
#version 330 core 
 
in vec3 vNormal; 
in vec3 vFragPos; 
in vec2 vUV; 
 
out vec4 FragColor; 
 
uniform vec3      u_viewPos;          // position caméra 
uniform sampler2D u_albedo; 
 
struct PointLight { 
    vec3  position; 
    vec3  color; 
    float constant, linear, quadratic; // atténuation 
}; 
uniform PointLight u_lights[4]; 
uniform int        u_numLights; 
 
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) { 
    vec3  lightDir  = normalize(light.position - fragPos); 
    vec3  halfDir   = normalize(lightDir + viewDir);   // Blinn 
 
    // Diffuse 
    float diff = max(dot(normal, lightDir), 0.0); 
    // Spéculaire (Blinn-Phong) 
    float spec = pow(max(dot(normal, halfDir), 0.0), 32.0); 
 
    // Atténuation 
    float d    = length(light.position - fragPos); 
    float att  = 1.0 / (light.constant + light.linear*d + light.quadratic*d*d); 
 
    return att * light.color * (diff + 0.1 * spec); // 0.1=specular strength 
} 
 
void main() { 
    vec3  N       = normalize(vNormal); 
    vec3  V       = normalize(u_viewPos - vFragPos); 
    vec3  albedo  = texture(u_albedo, vUV).rgb; 
    vec3  ambient = 0.03 * albedo; 
    vec3  light   = ambient; 
    for (int i = 0; i < u_numLights; i++) 
        light += calcPointLight(u_lights[i], N, vFragPos, V); 
    FragColor = vec4(light * albedo, 1.0); 
} 