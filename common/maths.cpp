#include <common/maths.hpp>

glm::mat4 Maths::translate(const glm::vec3& v)
{
    glm::mat4 translate(1.0f);
    translate[3][0] = v.x, translate[3][1] = v.y, translate[3][2] = v.z;
    return translate;
}
glm::mat4 Maths::scale(const glm::vec3& v)
{
    glm::mat4 scale(1.0f);
    scale[0][0] = v.x; scale[1][1] = v.y; scale[2][2] = v.z;
    return scale;
}
float Maths::radians(float angle)
{
    return angle * 3.1416f / 180.0f;
}

glm::mat4 Maths::rotate(const float& angle, glm::vec3 v)
{
    v = glm::normalize(v);
    float c = cos(0.5f * angle);
    float s = sin(0.5f * angle);
    Quaternion q(c, s * v.x, s * v.y, s * v.z);

    return q.matrix();
}
// Quaternions
Quaternion::Quaternion() {}

Quaternion::Quaternion(const float pitch, const float yaw)
{
    float cosPitch = cos(0.5f * pitch);
    float sinPitch = sin(0.5f * pitch);
    float cosYaw = cos(0.5f * yaw);
    float sinYaw = sin(0.5f * yaw);

    this->w = cosPitch * cosYaw;
    this->x = sinPitch * cosYaw;
    this->y = cosPitch * sinYaw;
    this->z = sinPitch * sinYaw;
}

Quaternion::Quaternion(const float w, const float x, const float y, const float z)
{
    this->w = w;
    this->x = x;
    this->y = y;
    this->z = z;
}
glm::mat4 Quaternion::matrix()
{
    float s = 2.0f / (w * w + x * x + y * y + z * z);
    float xs = x * s, ys = y * s, zs = z * s;
    float xx = x * xs, xy = x * ys, xz = x * zs;
    float yy = y * ys, yz = y * zs, zz = z * zs;
    float xw = w * xs, yw = w * ys, zw = w * zs;

    glm::mat4 rotate;
    rotate[0][0] = 1.0f - (yy + zz);
    rotate[0][1] = xy + zw;
    rotate[0][2] = xz - yw;
    rotate[1][0] = xy - zw;
    rotate[1][1] = 1.0f - (xx + zz);
    rotate[1][2] = yz + xw;
    rotate[2][0] = xz + yw;
    rotate[2][1] = yz - xw;
    rotate[2][2] = 1.0f - (xx + yy);

    return rotate;
}
// SLERP
Quaternion Maths::SLERP(Quaternion q1, Quaternion q2, const float t)
{
    // Calculate cos(theta)
    float cosTheta = q1.w * q2.w + q1.x * q2.x + q1.y * q2.y + q1.z * q2.z;

    // If q1 and q2 are close together return q2 to avoid divide by zero errors
    if (cosTheta > 0.9999f)
        return q2;

    // Avoid taking the long path around the sphere by reversing sign of q2
    if (cosTheta < 0)
    {
        q2 = Quaternion(-q2.w, -q2.x, -q2.y, -q2.z);
        cosTheta = -cosTheta;
    }

    // Calculate SLERP
    Quaternion q;
    float theta = acos(cosTheta);
    float a = sin((1.0f - t) * theta) / sin(theta);
    float b = sin(t * theta) / sin(theta);
    q.w = a * q1.w + b * q2.w;
    q.x = a * q1.x + b * q2.x;
    q.y = a * q1.y + b * q2.y;
    q.z = a * q1.z + b * q2.z;

    return q;
}