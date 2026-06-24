#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 OurColor;

// Alignement strict std140 (les types occupent des blocs de 16 octets)
layout (std140, binding = 0) uniform CommonData {
    vec2  u_Resolution;
    float u_Time;
    int   u_EffectIndex;
};

// --- VALUE NOISE GENERATOR ---
float hash(vec2 p) {
    p = fract(p * vec2(127.1, 311.7));
    p += dot(p, p + 19.19);
    return fract(p.x * p.y);
}

float valueNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);
    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(mix(hash(i + vec2(0.0,0.0)), hash(i + vec2(1.0,0.0)), u.x),
               mix(hash(i + vec2(0.0,1.0)), hash(i + vec2(1.0,1.0)), u.x), u.y);
}

// --- SDF (Signed Distance Fields) FUNCTIONS ---
float sdfCircle(vec2 p, float r) {
    return length(p) - r;
}

float sdfRectangle(vec2 p, vec2 b) {
    vec2 d = abs(p) - b;
    return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
}

void main()
{
    // Coordonnées de pixel recentrées [-1, 1] adaptées au ratio de l'écran
    vec2 uv = (gl_FragCoord.xy * 2.0 - u_Resolution.xy) / u_Resolution.y;
    vec3 color = vec3(0.0);

    // Effet 0 : SDF Cercle avec pulsation temporelle
    if (u_EffectIndex == 0) {
        float radius = 0.4 + 0.1 * sin(u_Time * 3.0);
        float d = sdfCircle(uv, radius);
        float alpha = smoothstep(0.01, 0.0, d);
        color = mix(vec3(0.08, 0.08, 0.15), vec3(0.1, 0.5, 1.0), alpha);
    }
    // Effet 1 : SDF Rectangle pur avec lissage des contours (Anti-aliasing)
    else if (u_EffectIndex == 1) {
        float d = sdfRectangle(uv, vec2(0.5, 0.3));
        float alpha = smoothstep(0.01, 0.0, d);
        color = mix(vec3(0.1, 0.1, 0.12), vec3(0.85, 0.15, 0.35), alpha);
    }
    // Effet 2 : Fractale de Mandelbrot
    else if (u_EffectIndex == 2) {
        vec2 c = uv * 1.3 - vec2(0.5, 0.0);
        vec2 z = vec2(0.0);
        float iter = 0.0;
        const float max_iter = 64.0;
        
        for (float i = 0.0; i < max_iter; i++) {
            z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
            if (dot(z, z) > 4.0) { iter = i; break; }
        }
        if (iter < max_iter) {
            color = 0.5 + 0.5 * cos(3.0 + iter * 0.12 + vec3(0.0, 0.5, 1.0));
        }
    }
    // Effet 3 : Value Noise (Génération de textures brumeuses/orages dynamiques)
    else if (u_EffectIndex == 3) {
        vec2 noiseUV = uv * 6.0 + vec2(u_Time * 0.4);
        float n = valueNoise(noiseUV);
        color = vec3(n * 0.8, n * 0.9, n);
    }
    // Effet 4 : UV Scroll (Défilement continu de grilles matricielles)
    else if (u_EffectIndex == 4) {
        vec2 scrolledUV = TexCoords + vec2(u_Time * 0.15, u_Time * 0.08);
        float grid = sin(scrolledUV.x * 25.0) * sin(scrolledUV.y * 25.0);
        color = (grid > 0.0) ? vec3(0.0, 0.8, 0.5) : vec3(0.05, 0.05, 0.1);
    }
    // Effet 5 : Morphing Animé (Interpolation linéaire de champs de distances SDF)
    else if (u_EffectIndex == 5) {
        float factor = sin(u_Time * 2.0) * 0.5 + 0.5; // Oscillation [0, 1]
        float dCircle = sdfCircle(uv, 0.45);
        float dRect = sdfRectangle(uv, vec2(0.4));
        float finalDist = mix(dCircle, dRect, factor);
        
        float alpha = smoothstep(0.01, 0.0, finalDist);
        color = mix(vec3(0.1), vec3(0.95, 0.75, 0.15), alpha);
    }

    FragColor = vec4(color, 1.0);
}