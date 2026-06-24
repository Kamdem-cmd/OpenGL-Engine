#pragma once

#include "NKGlad/include/glad/gl.h"

class UniformBuffer {
private:
    unsigned int m_ID;
    size_t m_Size;

public:
    UniformBuffer();
    void Init(size_t size, unsigned int bindingPoint);
    void Update(size_t offset, size_t size, const void* data);
    void Destroy();
};