#pragma once

#include "../nkMath/NkVec3.h"

using NkVec3f = NkVec3<float>;

class Camera {
public:
    NkVec3f position;
    NkVec3f forward;
    NkVec3f up;
    NkVec3f right;

    float yaw;
    float pitch;
    float fov;
    float speed;
    float sensitivity;

    Camera();
    void UpdateVectors();
    void ProcessKeyboard(int direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset);
    
    // Génère des matrices simulées sous forme de tableaux pour uniformiser l'envoi vers l'UBO std140
    void GetViewMatrix(float* outMatrix) const;
    void GetProjectionMatrix(float aspect, float* outMatrix) const;
};