#pragma once

#include <cmath>

struct Object {
    Mesh mesh;
    
    // Matrice de transformation finale (ou composants séparés)
    float position[3] = { 0.0f, 0.0f, 0.0f };
    float rotation[3] = { 0.0f, 0.0f, 0.0f }; // X, Y, Z en degrés
    float scale[3]    = { 1.0f, 1.0f, 1.0f };
    
    GLuint textureID  = 0;

    float tint[3]     = { 1.0f, 1.0f, 1.0f }; // Teinte multipliée sur la couleur/texture
    bool useTexture   = true;                  // false = couleur unie (teinte seule, pas de texture)
};

Object CreateCube3D(GLuint textureID) {
    Object obj;
    obj.textureID = textureID;

    // 24 sommets (6 faces * 4 sommets) pour un texturage et des normales sans bavure
    std::vector<Vertex> vertices = {
        // Position (NkVec3f), Couleur (NkVec3f) [Ici blanc], U, V
        
        // --- Face Avant (Z+) --- Normal (0, 0, 1)
        { NkVec3f(-0.5f, -0.5f,  0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 0.0f, 0.0f },
        { NkVec3f( 0.5f, -0.5f,  0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 1.0f, 0.0f },
        { NkVec3f( 0.5f,  0.5f,  0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 1.0f, 1.0f },
        { NkVec3f(-0.5f,  0.5f,  0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 0.0f, 1.0f },

        // --- Face Arrière (Z-) --- Normal (0, 0, -1)
        { NkVec3f( 0.5f, -0.5f, -0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 0.0f, 0.0f },
        { NkVec3f(-0.5f, -0.5f, -0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 1.0f, 0.0f },
        { NkVec3f(-0.5f,  0.5f, -0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 1.0f, 1.0f },
        { NkVec3f( 0.5f,  0.5f, -0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 0.0f, 1.0f },

        // --- Face Gauche (X-) --- Normal (-1, 0, 0)
        { NkVec3f(-0.5f, -0.5f, -0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 0.0f, 0.0f },
        { NkVec3f(-0.5f, -0.5f,  0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 1.0f, 0.0f },
        { NkVec3f(-0.5f,  0.5f,  0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 1.0f, 1.0f },
        { NkVec3f(-0.5f,  0.5f, -0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 0.0f, 1.0f },

        // --- Face Droite (X+) --- Normal (1, 0, 0)
        { NkVec3f( 0.5f, -0.5f,  0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 0.0f, 0.0f },
        { NkVec3f( 0.5f, -0.5f, -0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 1.0f, 0.0f },
        { NkVec3f( 0.5f,  0.5f, -0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 1.0f, 1.0f },
        { NkVec3f( 0.5f,  0.5f,  0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 0.0f, 1.0f },

        // --- Face Haut (Y+) --- Normal (0, 1, 0)
        { NkVec3f(-0.5f,  0.5f,  0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 0.0f, 0.0f },
        { NkVec3f( 0.5f,  0.5f,  0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 1.0f, 0.0f },
        { NkVec3f( 0.5f,  0.5f, -0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 1.0f, 1.0f },
        { NkVec3f(-0.5f,  0.5f, -0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 0.0f, 1.0f },

        // --- Face Bas (Y-) --- Normal (0, -1, 0)
        { NkVec3f(-0.5f, -0.5f, -0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 0.0f, 0.0f },
        { NkVec3f( 0.5f, -0.5f, -0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 1.0f, 0.0f },
        { NkVec3f( 0.5f, -0.5f,  0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 1.0f, 1.0f },
        { NkVec3f(-0.5f, -0.5f,  0.5f), NkVec3f(1.0f, 1.0f, 1.0f), 0.0f, 1.0f }
    };

    // Indices CCW pour les 12 triangles (6 faces * 2 triangles)
    std::vector<unsigned int> indices = {
        0,  1,  2,   2,  3,  0,       // Face Avant
        4,  5,  6,   6,  7,  4,       // Face Arrière
        8,  9,  10,  10, 11, 8,       // Face Gauche
        12, 13, 14,  14, 15, 12,      // Face Droite
        16, 17, 18,  18, 19, 16,      // Face Haut
        20, 21, 22,  22, 23, 20       // Face Bas
    };

    obj.mesh.Init(vertices, indices);
    return obj;
}

// =============================================================================
// CreateSphere3D — UV-sphère paramétrique (texturée, lisse via duplication
// de sommets aux pôles/méridiens pour un mapping UV propre).
// =============================================================================
Object CreateSphere3D(GLuint textureID, unsigned int stacks = 16, unsigned int slices = 24) {
    Object obj;
    obj.textureID = textureID;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    const float PI = 3.14159265358979323846f;

    for (unsigned int i = 0; i <= stacks; ++i) {
        float v = (float)i / (float)stacks;          // 0 (pôle nord) -> 1 (pôle sud)
        float phi = v * PI;                           // 0 -> PI
        float y = std::cos(phi);
        float ringRadius = std::sin(phi);

        for (unsigned int j = 0; j <= slices; ++j) {
            float u = (float)j / (float)slices;        // 0 -> 1 autour de l'axe Y
            float theta = u * 2.0f * PI;
            float x = ringRadius * std::cos(theta);
            float z = ringRadius * std::sin(theta);

            Vertex vert;
            vert.position = NkVec3f(x * 0.5f, y * 0.5f, z * 0.5f); // rayon 0.5, comme le cube
            vert.color = NkVec3f(1.0f, 1.0f, 1.0f);
            vert.u = u;
            vert.v = v;
            vertices.push_back(vert);
        }
    }

    for (unsigned int i = 0; i < stacks; ++i) {
        for (unsigned int j = 0; j < slices; ++j) {
            unsigned int a = i * (slices + 1) + j;
            unsigned int b = a + slices + 1;

            // Deux triangles par "quad" de la grille stacks x slices
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(a + 1);

            indices.push_back(a + 1);
            indices.push_back(b);
            indices.push_back(b + 1);
        }
    }

    obj.mesh.Init(vertices, indices);
    return obj;
}

// =============================================================================
// CreatePyramid3D — base carrée + 4 faces triangulaires (style "pyramide égyptienne").
// Faces séparées (pas de partage de sommets) pour un éclairage/texturage propre.
// =============================================================================
Object CreatePyramid3D(GLuint textureID) {
    Object obj;
    obj.textureID = textureID;

    const NkVec3f apex(0.0f, 0.5f, 0.0f);
    const NkVec3f baseFL(-0.5f, -0.5f,  0.5f); // avant-gauche
    const NkVec3f baseFR( 0.5f, -0.5f,  0.5f); // avant-droite
    const NkVec3f baseBR( 0.5f, -0.5f, -0.5f); // arrière-droite
    const NkVec3f baseBL(-0.5f, -0.5f, -0.5f); // arrière-gauche

    const NkVec3f white(1.0f, 1.0f, 1.0f);

    std::vector<Vertex> vertices = {
        // --- Face Avant ---
        { baseFL, white, 0.0f, 0.0f },
        { baseFR, white, 1.0f, 0.0f },
        { apex,   white, 0.5f, 1.0f },

        // --- Face Droite ---
        { baseFR, white, 0.0f, 0.0f },
        { baseBR, white, 1.0f, 0.0f },
        { apex,   white, 0.5f, 1.0f },

        // --- Face Arrière ---
        { baseBR, white, 0.0f, 0.0f },
        { baseBL, white, 1.0f, 0.0f },
        { apex,   white, 0.5f, 1.0f },

        // --- Face Gauche ---
        { baseBL, white, 0.0f, 0.0f },
        { baseFL, white, 1.0f, 0.0f },
        { apex,   white, 0.5f, 1.0f },

        // --- Base (carrée, vue du dessous, Normal (0,-1,0)) ---
        { baseFL, white, 0.0f, 0.0f },
        { baseBL, white, 1.0f, 0.0f },
        { baseBR, white, 1.0f, 1.0f },
        { baseFR, white, 0.0f, 1.0f },
    };

    std::vector<unsigned int> indices = {
        0,  1,  2,        // Avant
        3,  4,  5,        // Droite
        6,  7,  8,        // Arrière
        9,  10, 11,       // Gauche
        12, 13, 14, 14, 15, 12 // Base
    };

    obj.mesh.Init(vertices, indices);
    return obj;
}

// =============================================================================
// CreateQuad2D — quad plat (2 triangles) dans le plan XY, face vers +Z.
// Primitive 2D rendue avec le même pipeline 3D que le cube (mêmes shaders/UBO).
// =============================================================================
Object CreateQuad2D(GLuint textureID) {
    Object obj;
    obj.textureID = textureID;

    const NkVec3f white(1.0f, 1.0f, 1.0f);

    std::vector<Vertex> vertices = {
        { NkVec3f(-0.5f, -0.5f, 0.0f), white, 0.0f, 0.0f },
        { NkVec3f( 0.5f, -0.5f, 0.0f), white, 1.0f, 0.0f },
        { NkVec3f( 0.5f,  0.5f, 0.0f), white, 1.0f, 1.0f },
        { NkVec3f(-0.5f,  0.5f, 0.0f), white, 0.0f, 1.0f },
    };

    std::vector<unsigned int> indices = { 0, 1, 2,  2, 3, 0 };

    obj.mesh.Init(vertices, indices);
    return obj;
}

// =============================================================================
// CreateTriangle2D — triangle plat dans le plan XY, face vers +Z.
// =============================================================================
Object CreateTriangle2D(GLuint textureID) {
    Object obj;
    obj.textureID = textureID;

    const NkVec3f white(1.0f, 1.0f, 1.0f);

    std::vector<Vertex> vertices = {
        { NkVec3f( 0.0f,  0.5f, 0.0f), white, 0.5f, 1.0f }, // sommet haut
        { NkVec3f(-0.5f, -0.5f, 0.0f), white, 0.0f, 0.0f }, // bas-gauche
        { NkVec3f( 0.5f, -0.5f, 0.0f), white, 1.0f, 0.0f }, // bas-droite
    };

    obj.mesh.Init(vertices); // pas d'indices : un seul triangle, dessiné tel quel
    return obj;
}