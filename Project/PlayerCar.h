#pragma once
#include "Math.h"
#include "Shader.h"

void drawCarModel(const Shader& shader, const Mat4& base, Vec3 bodyColor, bool lightsOn,
    int style = 0, float wheelAngle = 0.0f, bool braking = false);

class PlayerCar
{
public:
    static constexpr float LANE_WIDTH = 3.5f;
    static constexpr float MAX_SPEED = 45.0f;

    Vec3  pos{ 0, 0, 0 };
    float speed = 0.0f;
    int   lane = 0;

    void  reset();
    void  changeLane(int dir);
    void  update(float dt, bool throttle, bool brake);
    void  draw(const Shader& shader, bool lightsOn) const;
    float speedKmh() const { return speed * 3.6f; }

private:
    float yaw = 0.0f, roll = 0.0f, pitch = 0.0f;
    float latVel = 0.0f;
    bool  braking = false;
};