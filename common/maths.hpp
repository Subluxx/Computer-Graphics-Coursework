#pragma once

#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtx/io.hpp>

// Quaternion class a
class Quaternion
{
public:
    float w, x, y, z;
    glm::mat4 matrix();

    // Constructors

    Quaternion();
    Quaternion(const float pitch, const float yaw);
    Quaternion(const float w, const float x, const float y, const float z);


};

// Maths class
class Maths
{
public:
    // Transformation matrices
    static glm::mat4 translate(const glm::vec3& v);
    //scaling matrices
    static glm::mat4 scale(const glm::vec3& v);
    static float radians(float angle);
    static glm::mat4 rotate(const float& angle, glm::vec3 v);
    static Quaternion SLERP(const Quaternion q1, const Quaternion q2, const float t);

};
