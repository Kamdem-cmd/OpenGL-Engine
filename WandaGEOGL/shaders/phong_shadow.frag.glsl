 
// Étape 2 : utiliser la shadow map dans le fragment shader principal 
// shaders/phong_shadow.frag 
uniform sampler2D u_shadowMap; 
uniform mat4      u_lightSpaceMVP; 
 
float shadowTest(vec3 fragPos) { 
    vec4 lsPos   = u_lightSpaceMVP * vec4(fragPos, 1.0); 
    vec3 projCoords = lsPos.xyz / lsPos.w;  // perspective divide 
    projCoords = projCoords * 0.5 + 0.5;    // [-1,1] → [0,1] 
 
    if (projCoords.z > 1.0) return 0.0;     // hors du frustum lumière 
 
    float closestDepth = texture(u_shadowMap, projCoords.xy).r; 
    float currentDepth  = projCoords.z; 
 
    // PCF : noyau 3×3 pour adoucir les ombres 
    float shadow = 0.0; 
    vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0); 
    for (int x=-1; x<=1; x++) for (int y=-1; y<=1; y++) { 
        float pcfDepth = texture(u_shadowMap, projCoords.xy + 
        vec2(x,y)*texelSize).r; 
        shadow += currentDepth - 0.005 > pcfDepth ? 1.0 : 0.0; // bias=0.005 
    } 
    return shadow / 9.0; 
} 