#include "Camera.h"
#include <cmath>
#include <algorithm>

Vec3 Camera::front() const
{
    float cy = std::cos(radians(yaw)), sy = std::sin(radians(yaw));
    float cp = std::cos(radians(pitch)), sp = std::sin(radians(pitch));
    return Vec3(cy * cp, sp, sy * cp);
}

void Camera::follow(Vec3 target, float speed, float dt)
{
    if (freeMode) return;
    float dist = 8.0f + speed * 0.08f;
    Vec3 desired(target.x * 0.6f, target.y + 3.6f, target.z + dist);

    float kx = std::min(1.0f, 2.5f * dt);     
    float kyz = std::min(1.0f, 6.0f * dt);
    position.x += (desired.x - position.x) * kx;
    position.y += (desired.y - position.y) * kyz;
    position.z += (desired.z - position.z) * kyz;

    lookTarget.x += (target.x - lookTarget.x) * std::min(1.0f, 4.0f * dt);
    lookTarget.y = target.y + 1.0f;
    lookTarget.z = target.z - 8.0f;

    fov += ((60.0f + speed * 0.3f) - fov) * std::min(1.0f, 3.0f * dt);
}


void Camera::enterFree()
{
    Vec3 d = normalize(lookTarget - position);
    pitch = std::asin(d.y) * 57.29578f;
    yaw = std::atan2(d.z, d.x) * 57.29578f;
    freeMode = true;
}

void Camera::freeMove(float dt, float fwd, float right, float up, bool fast)
{
    float sp = (fast ? 60.0f : 20.0f) * dt;
    Vec3 f = front();
    Vec3 r = normalize(cross(f, Vec3(0, 1, 0)));
    position = position + f * (fwd * sp) + r * (right * sp) + Vec3(0, 1, 0) * (up * sp);
}

void Camera::freeLook(float dx, float dy)
{
    yaw += dx * 0.1f;
    pitch -= dy * 0.1f;
    pitch = std::max(-89.0f, std::min(89.0f, pitch));
}

Mat4 Camera::view() const
{
    if (freeMode) return lookAt(position, position + front(), Vec3(0, 1, 0));
    return lookAt(position, lookTarget, Vec3(0, 1, 0));
}

Mat4 Camera::projection(float aspect) const
{
    return perspective(radians(fov), aspect, 0.1f, 400.0f);
}