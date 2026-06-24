#pragma once

#include "NKGlad/include/glad/gl.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cerrno>

std::string getFileContent(const char *filename);

class Shader {
public:
    GLuint ID;

    Shader();
    Shader(const char* vertexSource, const char* fragmentSource);
    void Destroy();

    void use() const;
    bool isSet() const;

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec3(const std::string &name, const float* v) const;
    void setMat4(const std::string &name, const float* matrixData) const;
};