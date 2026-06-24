#include "ShaderProgram.h"
#include <iostream>
#include <fstream>

Shader::Shader() : ID(0) {}

std::string getFileContent(const char *filename)
{
    std::ifstream in(filename, std::ios::binary);
    if (in)
    {
        std::string Content;
        in.seekg(0, std::ios::end);
        Content.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&Content[0], Content.size());
        in.close();
        return (Content);
    }
    throw(errno);
}

Shader::Shader(const char* vertexSource, const char* fragmentSource){
    std::string vertex = getFileContent(vertexSource);
    std::string fragment = getFileContent(fragmentSource);

    const char *vertexShaderSource = vertex.c_str();
    const char *fragmentShaderSource = fragment.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Destroy() {
    if (ID != 0) {
        glDeleteProgram(ID);
        ID = 0;
    }
}

void Shader::use() const {
    if (ID != 0) glUseProgram(ID);
}

bool Shader::isSet() const {
    return ID != 0;
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    // CORRECTION : glUniform1f au lieu de glUniform1i
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
}

// ============================================================
// Nouvelle méthode : Envoi d'un vecteur 3 (ex: couleur/teinte) à un uniform
// ============================================================
void Shader::setVec3(const std::string &name, const float* v) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, v);
}

// ============================================================
// Nouvelle méthode : Envoi d'une matrice 4x4 à un uniform
// ============================================================
void Shader::setMat4(const std::string &name, const float* matrixData) const {
    // GL_FALSE car NkMat4f::ToPtr se charge déjà de la transposition Row -> Column Major
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, matrixData);
}