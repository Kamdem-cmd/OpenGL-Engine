// ── Cercle SDF avec antialiasing ────────────────────── 
#version 460 core 
in  vec2 vUV; 
out vec4 FragColor; 
uniform float u_time; 
 
float sdCircle(vec2 p, float r) { return length(p) - r; } 
 
void main() { 
    vec2 p = vUV * 2.0 - 1.0;  // [0,1] → [-1,1] 
    float d = sdCircle(p, 0.5 + 0.1*sin(u_time)); 
 
    // Antialiasing via smoothstep (1 pixel de transition) 
    float aa = fwidth(d);  // dérivée de d pour l'AA 
    float circle = smoothstep(aa, -aa, d); 
 
    vec3 col = mix(vec3(0.1, 0.1, 0.2), vec3(0.9, 0.4, 0.1), circle); 
    FragColor = vec4(col, 1.0); 
} 
 
// ── Bruit procédural (hash + value noise) ────────────── 
float hash(vec2 p) { 
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453); 
} 
float valueNoise(vec2 p) { 
    vec2 i = floor(p); vec2 f = fract(p); 
    vec2 u = f*f*(3.0-2.0*f);  // smoothstep 
    return mix(mix(hash(i),         hash(i+vec2(1,0)), u.x),
    mix(hash(i+vec2(0,1)),hash(i+vec2(1,1)), u.x), u.y); 
} 