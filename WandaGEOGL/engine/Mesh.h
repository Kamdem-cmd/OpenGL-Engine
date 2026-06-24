#pragma once

#include "NKGlad/include/glad/gl.h"
#include "../nkMath/NkVec3.h"
#include "Vertex.h"
#include <vector>

class Mesh {
private:
    unsigned int m_VAO, m_VBO, m_EBO;
    size_t m_VertexCount;
    size_t m_IndexCount;
    bool m_Indexed;

public:
    Mesh();
    void Init(const std::vector<Vertex>& vertices);
    void Init(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void Draw() const;
    void Destroy();

    size_t GetVertexCount() const { return m_VertexCount; }
    size_t GetTriangleCount() const { return m_Indexed ? (m_IndexCount / 3) : (m_VertexCount / 3); }
};