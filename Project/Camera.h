#pragma once
#include "Math.h"

class Camera
{
public:
    Vec3 position{ 0, 4, 9 };
    bool freeMode = false;
    float fov = 60.0f;

    void follow(Vec3 target, float speed, float dt);          
    void enterFree();                                          
    void exitFree() { freeMode = false; }
    void freeMove(float dt, float fwd, float right, float up, bool fast);
    void freeLook(float dx, float dy);
    Mat4 view() const;
    Mat4 projection(float aspect) const;

private:
    Vec3  lookTarget{ 0, 1, 0 };
    float yaw = -90.0f, pitch = 0.0f;
    Vec3  front() const;
};