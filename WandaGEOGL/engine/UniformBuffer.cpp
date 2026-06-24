#include "UniformBuffer.h"

UniformBuffer::UniformBuffer() : m_ID(0), m_Size(0) {}

void UniformBuffer::Init(size_t size, unsigned int bindingPoint) {
    m_Size = size;
    glGenBuffers(1, &m_ID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_ID);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::Update(size_t offset, size_t size, const void* data) {
    glBindBuffer(GL_UNIFORM_BUFFER, m_ID);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::Destroy() {
    if (m_ID != 0) {
        glDeleteBuffers(1, &m_ID);
        m_ID = 0;
    }
}