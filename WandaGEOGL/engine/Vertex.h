#pragma once

#include "../nkMath/NkVec3.h"
using NkVec3f = NkVec3<float>;

struct Vertex {
    NkVec3f position;
    NkVec3f color;
    float u, v; // Coordonnées UV intégrées natives
};

