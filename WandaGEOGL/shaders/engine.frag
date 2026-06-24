#version 460 core
out vec4 FragColor;

in vec3 OurColor;
in vec2 TexCoords;

layout (std140, binding = 0) uniform CommonData {
    vec2  u_Resolution;
    float u_Time;
    float u_DeltaTime;
    int   u_EffectIndex;
    float u_SpeedMultiplier;
    mat4  u_View;
    mat4  u_Proj;
};

// 1. DÉCLARATION DU SAMPLER DE TEXTURE
uniform sampler2D u_TextureSampler;
uniform bool u_UseTexture = true; // Bascule par objet : texture ou couleur unie (teinte seule)

float valueNoise(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453123);
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2.0 - u_Resolution.xy) / u_Resolution.y;
    float animatedTime = u_Time * u_SpeedMultiplier;

    // 2. ÉCHANTILLONNER LA TEXTURE PAR DÉFAUT (ou couleur unie si désactivée)
    // OurColor porte déjà la teinte de l'objet (cf. vertex shader : aColor * u_Tint).
    vec3 outColor;
    if (u_UseTexture) {
        vec4 texColor = texture(u_TextureSampler, TexCoords);
        outColor = texColor.rgb * OurColor;
    } else {
        outColor = OurColor;
    }

    // --- EFFETS PROCÉDURAUX (Conservés) ---
    // Index 0 = Aucun effet, on garde la texture brute déjà calculée dans outColor.
    if (u_EffectIndex == 0) {
        // Rien à faire : outColor contient déjà texColor.rgb * OurColor
    }
    else if (u_EffectIndex == 1) { // SDF Cercle Pulsé
        float d = length(uv) - (0.3 + 0.1 * sin(animatedTime * 3.0));
        // On mixe désormais la TEXTURE avec le cercle bleu au lieu du blanc uni
        outColor = mix(outColor, vec3(0.0, 0.4, 0.9), smoothstep(0.01, 0.0, d));
    } 
    else if (u_EffectIndex == 2) { // SDF Rectangle
        vec2 d = abs(uv) - vec2(0.4, 0.2);
        float field = length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
        outColor = mix(outColor, vec3(0.9, 0.1, 0.3), smoothstep(0.01, 0.0, field));
    }
    else if (u_EffectIndex == 3) { // Mandelbrot Fractale
        vec2 c = uv * 1.5 - vec2(0.5, 0.0);
        vec2 z = vec2(0.0);
        float iter = 0.0;
        for (float i = 0.0; i < 30.0; i++) {
            z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + c;
            if (dot(z, z) > 4.0) { iter = i; break; }
        }
        outColor = vec3(iter / 30.0) * vec3(0.2, 0.8, 0.5);
    }
    else if (u_EffectIndex == 4) { // Value Noise
        float n = valueNoise(uv * 10.0 + animatedTime);
        outColor = vec3(n) * outColor;
    }
    else if (u_EffectIndex == 5) { // Scrolling UV Grille (Modifié pour défiler votre texture !)
        vec2 scrolled = TexCoords + vec2(animatedTime * 0.1);
        vec4 scrolledTex = texture(u_TextureSampler, scrolled);
        float line = sin(scrolled.x * 20.0) * sin(scrolled.y * 20.0);
        outColor = (line > 0.0) ? vec3(0.1, 0.9, 0.6) : scrolledTex.rgb * OurColor * 0.2;
    }
    else if (u_EffectIndex == 6) { // Morphing de Formes
        float t = sin(animatedTime) * 0.5 + 0.5;
        float c_d = length(uv) - 0.35;
        vec2 r_box = abs(uv) - vec2(0.3);
        float r_d = length(max(r_box, 0.0)) + min(max(r_box.x, r_box.y), 0.0);
        outColor = mix(outColor, vec3(0.9, 0.8, 0.1), smoothstep(0.01, 0.0, mix(c_d, r_d, t)));
    }

    FragColor = vec4(outColor, 1.0);
}