#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec2 TexCoords;
out vec3 OurColor;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    OurColor = aColor;
    // Mappe les positions de [-1, 1] vers des coordonnées UV normales [0, 1]
    TexCoords = aPos.xy * 0.5 + 0.5;
}