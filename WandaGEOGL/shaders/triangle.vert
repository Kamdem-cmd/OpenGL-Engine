#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;

void main()
{
    // Passage direct du vec3 au vec4 (plus robuste et standard)
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
}