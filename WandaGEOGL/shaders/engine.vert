#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoords;

out vec3 OurColor;
out vec2 TexCoords;

layout (std140, binding = 0) uniform CommonData {
    vec2  u_Resolution;
    float u_Time;
    float u_DeltaTime;
    int   u_EffectIndex;
    float u_SpeedMultiplier;
    mat4  u_View;
    mat4  u_Proj;
};

uniform mat4 u_Model; // À ajouter en uniform classique
uniform vec3 u_Tint = vec3(1.0, 1.0, 1.0); // Teinte par objet, multipliée sur la couleur du sommet

void main() {
    // On applique le modèle AVANT la vue et la projection
    gl_Position = u_Proj * u_View * u_Model * vec4(aPos, 1.0);
    OurColor = aColor * u_Tint;
    TexCoords = aTexCoords;
}