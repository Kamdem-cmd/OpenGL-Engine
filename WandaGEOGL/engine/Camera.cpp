#include "Camera.h"
#include <cmath>

Camera::Camera() {
    position = NkVec3f(0.0f, 0.0f, 3.0f);
    forward = NkVec3f(0.0f, 0.0f, -1.0f);
    up = NkVec3f(0.0f, 1.0f, 0.0f);
    right = NkVec3f(1.0f, 0.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    fov = 45.0f;
    speed = 2.5f;
    sensitivity = 0.1f;
    UpdateVectors();
}

void Camera::UpdateVectors() {
    float radYaw = yaw * 0.01745329251f;
    float radPitch = pitch * 0.01745329251f;

    // 1. Calcul du vecteur directionnel brut (front) à l'aide du constructeur
    NkVec3f front(
        cos(radYaw) * cos(radPitch),
        sin(radPitch),
        sin(radYaw) * cos(radPitch)
    );
    
    // 2. Utilisation de la méthode de normalisation native de NkVec3
    forward = front.Normalize();

    // 3. Calcul orthogonaux de Right & Up avec CrossProduct native
    NkVec3f worldUp(0.0f, 1.0f, 0.0f);
    right = forward.CrossProduct(worldUp).Normalize();
}

void Camera::ProcessKeyboard(int direction, float deltaTime) {
    float velocity = speed * deltaTime;
    
    // Utilisation des opérateurs de surcharge + et * de NkVec3
    if (direction == 0) position = position + (forward * velocity); // Avancer (W)
    if (direction == 1) position = position - (forward * velocity); // Reculer (S)
    if (direction == 2) position = position - (right * velocity);   // Gauche (A)
    if (direction == 3) position = position + (right * velocity);   // Droite (D)
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset) {
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    yaw += xoffset;
    pitch += yoffset;
    
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    
    UpdateVectors();
}

void Camera::GetViewMatrix(float* m) const {
    // Matrice Identité simulée avec Translation 
    for(int i = 0; i < 16; ++i) m[i] = (i % 5 == 0) ? 1.0f : 0.0f;
    
    // Accès aux données via l'opérateur [] public de NkVec3
    m[12] = -position[0]; // mX
    m[13] = -position[1]; // mY
    m[14] = -position[2]; // mZ
}

void Camera::GetProjectionMatrix(float aspect, float* m) const {
    float f = 1.0f / tan((fov * 0.5f) * 0.01745329251f);
    for(int i = 0; i < 16; ++i) m[i] = 0.0f;
    m[0] = f / aspect;
    m[5] = f;
    m[10] = -1.01f;
    m[11] = -1.0f;
    m[14] = -0.2f;
}