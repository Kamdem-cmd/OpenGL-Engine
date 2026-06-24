#pragma once

#include "NKGlad/include/glad/wgl.h"
#include "NKGlad/include/glad/gl.h"
#include "stb_image.h"

#include <cstdlib>

// Classe Texture2D 
class Texture2D { 
public: 
    GLuint id = 0; 
    int width = 0, height = 0; 
 
    bool load(const char* path, bool flip = true) { 
        stbi_set_flip_vertically_on_load(flip); // OpenGL = Y inversé 
        int channels; 
        uint8_t* data = stbi_load(path, &width, &height, &channels, 4); 
        if (!data) return false; 
 
        glGenTextures(1, &id); 
        glBindTexture(GL_TEXTURE_2D, id); 
 
        // Paramètres de filtering 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
GL_LINEAR_MIPMAP_LINEAR); 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
 
        // Upload CPU→GPU 
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, 
                     GL_RGBA, GL_UNSIGNED_BYTE, data); 
        glGenerateMipmap(GL_TEXTURE_2D); // auto-générer les mipmaps 
 
        stbi_image_free(data); 
        return true; 
    } 
 
    void bind(int unit = 0) const { 
        glActiveTexture(GL_TEXTURE0 + unit); 
        glBindTexture(GL_TEXTURE_2D, id); 
    } 
}; 
 
