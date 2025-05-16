#pragma once

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <common/maths.hpp>

class Camera
{
public:
    // Projection parameters
    float fov = Maths::radians(45.0f);
    float aspect = 1024.0f / 768.0f;
    float near = 0.2f;
    float far = 100.0f;

    // Camera vectors
    glm::vec3 eye;
    glm::vec3 target;
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);

    // Transformation matrices
    glm::mat4 view;
    glm::mat4 projection;

    // Camera Euler angles
    float yaw = 0.0f;
    float pitch = 0.0f;
    float roll = 0.0f;

    // Quaternion camera
    Quaternion orientation = Quaternion(pitch, yaw);

    // Constructor
    Camera(const glm::vec3 eye, const glm::vec3 target);

    // Methods
    void calculateMatrices();
    void calculateCameraVectors();
    void quaternionCamera();
};
