#include "PlayerCar.h"
#include "Mesh.h"
#include <algorithm>
#include <cmath>

namespace
{
    struct CarStyle
    {
        float clear, bodyH, wheelR;                    
        Mesh::Shape cabin;
        float cabinW, cabinH, cabinLen, cabinZ;        
        float cargoH, cargoLen, cargoZ;               
    };

    const CarStyle STYLES[4] = {
        
        { 0.30f, 0.60f, 0.33f, Mesh::CabinSedan, 1.60f, 0.55f, 2.20f,  0.30f, 0.0f, 0.0f, 0.0f },
        
        { 0.30f, 0.60f, 0.33f, Mesh::CabinHatch, 1.60f, 0.60f, 2.40f,  0.40f, 0.0f, 0.0f, 0.0f },
        
        { 0.40f, 0.75f, 0.42f, Mesh::CabinSUV,   1.70f, 0.70f, 2.60f,  0.30f, 0.0f, 0.0f, 0.0f },

        { 0.35f, 0.65f, 0.38f, Mesh::CabinSUV,   1.70f, 0.70f, 1.50f, -1.05f, 1.15f, 2.30f, 0.80f },
    };
}

void drawCarModel(const Shader& s, const Mat4& base, Vec3 body, bool lightsOn,
    int style, float wheelAngle, bool braking)
{
    const CarStyle& c = STYLES[((style % 4) + 4) % 4];
    const Vec3 glass(0.16f, 0.22f, 0.32f), dark(0.05f, 0.05f, 0.06f), chrome(0.75f, 0.75f, 0.78f);
    const float topBody = c.clear + c.bodyH;

    Mesh::draw(s, Mesh::Body, base * translate(Vec3(0, c.clear + c.bodyH * 0.5f, 0)) * scale(Vec3(1.8f, c.bodyH, 4.0f)), body);

    Mesh::drawBox(s, base * translate(Vec3(0, c.clear + 0.17f, -1.98f)) * scale(Vec3(1.84f, 0.24f, 0.16f)), dark);
    Mesh::drawBox(s, base * translate(Vec3(0, c.clear + 0.17f, 1.98f)) * scale(Vec3(1.84f, 0.24f, 0.16f)), dark);
    Mesh::drawBox(s, base * translate(Vec3(0, c.clear + 0.17f, -2.075f)) * scale(Vec3(0.5f, 0.13f, 0.03f)), Vec3(0.95f, 0.95f, 0.90f));
    Mesh::drawBox(s, base * translate(Vec3(0, c.clear + 0.17f, 2.075f)) * scale(Vec3(0.5f, 0.13f, 0.03f)), Vec3(0.95f, 0.95f, 0.90f));

    float ly = c.clear + c.bodyH * 0.6f;
    float hl = lightsOn ? 1.0f : 0.15f;
    float tl = braking ? 1.0f : (lightsOn ? 0.55f : 0.2f);
    for (int sx = -1; sx <= 1; sx += 2)
    {
        Mesh::drawBox(s, base * translate(Vec3(sx * 0.62f, ly, -1.96f)) * scale(Vec3(0.42f, 0.16f, 0.08f)), Vec3(1.0f, 0.95f, 0.75f), hl);
        Mesh::drawBox(s, base * translate(Vec3(sx * 0.62f, ly, 1.96f)) * scale(Vec3(0.42f, 0.16f, 0.08f)), Vec3(1.0f, 0.08f, 0.08f), tl);
    }

    float cy = topBody + c.cabinH * 0.5f;
    Mesh::draw(s, c.cabin, base * translate(Vec3(0, cy, c.cabinZ)) * scale(Vec3(c.cabinW, c.cabinH, c.cabinLen)), glass);
    Mesh::Taper t = Mesh::taper(c.cabin);
    float topW = c.cabinW * (1.0f - 2.0f * t.ix);
    float topL = c.cabinLen * (1.0f - t.fz - t.bz);
    float topZ = c.cabinZ + c.cabinLen * (t.fz - t.bz) * 0.5f;
    Mesh::drawBox(s, base * translate(Vec3(0, topBody + c.cabinH + 0.01f, topZ)) * scale(Vec3(topW + 0.04f, 0.06f, topL + 0.04f)), body);

    for (int sx = -1; sx <= 1; sx += 2)
        Mesh::drawBox(s, base * translate(Vec3(sx * 0.98f, topBody + 0.12f, c.cabinZ - c.cabinLen * 0.5f + 0.35f)) * scale(Vec3(0.16f, 0.10f, 0.18f)), body);

    if (c.cargoH > 0.0f)
        Mesh::drawBox(s, base * translate(Vec3(0, topBody + c.cargoH * 0.5f, c.cargoZ)) * scale(Vec3(1.8f, c.cargoH, c.cargoLen)), body);

    const float wx = 0.8f, wz = 1.28f, ww = 0.26f;
    for (int sx = -1; sx <= 1; sx += 2)
        for (int sz = -1; sz <= 1; sz += 2)
        {
            Mat4 wm = base * translate(Vec3(sx * wx, c.wheelR, sz * wz));
            Mesh::draw(s, Mesh::Cylinder, wm * scale(Vec3(ww, c.wheelR * 2.0f, c.wheelR * 2.0f)), dark);
            Mesh::draw(s, Mesh::Cylinder, wm * scale(Vec3(ww + 0.04f, c.wheelR * 1.1f, c.wheelR * 1.1f)), chrome);
            Mesh::drawBox(s, wm * translate(Vec3(sx * (ww * 0.5f + 0.025f), 0, 0)) * rotateX(wheelAngle)
                * scale(Vec3(0.02f, c.wheelR * 1.0f, 0.07f)), dark);
        }
}

void PlayerCar::reset()
{
    pos = Vec3(0, 0, 0);
    speed = 0.0f;
    lane = 0;
    yaw = roll = pitch = 0.0f;
    latVel = 0.0f;
    braking = false;
}

void PlayerCar::changeLane(int dir)
{
    lane = std::max(-1, std::min(1, lane + dir));
}

void PlayerCar::update(float dt, bool throttle, bool brake)
{
    float oldSpeed = speed;
    if (throttle) speed += 7.0f * dt;
    if (brake)    speed -= 22.0f * dt;
    if (!throttle && !brake) speed -= 3.0f * dt;
    speed = std::max(0.0f, std::min(MAX_SPEED, speed));
    braking = brake && speed > 0.5f;

    pos.z -= speed * dt;

    float dx = lane * LANE_WIDTH - pos.x;
    float ctrl = clampf(speed / 5.0f, 0.25f, 1.0f);
    float desiredV = clampf(dx * 2.2f, -4.5f * ctrl, 4.5f * ctrl);
    float dv = clampf(desiredV - latVel, -14.0f * dt, 14.0f * dt);
    latVel += dv;
    pos.x += latVel * dt;

    float accel = (dt > 0.0f) ? (speed - oldSpeed) / dt : 0.0f;
    float k = std::min(1.0f, 6.0f * dt);
    pitch += (clampf(accel * 0.0035f, -0.05f, 0.04f) - pitch) * k;
    roll += (clampf(latVel * 0.012f, -0.05f, 0.05f) - roll) * k;

    float targetYaw = clampf(-std::atan2(latVel, std::max(speed, 8.0f)), -0.3f, 0.3f);
    yaw += (targetYaw - yaw) * std::min(1.0f, 8.0f * dt);
}

void PlayerCar::draw(const Shader& shader, bool lightsOn) const
{
    Mat4 base = translate(pos) * rotateY(yaw) * rotateZ(roll) * rotateX(pitch);
    float wheelAngle = std::fmod(pos.z / 0.35f, 6.2831853f);
    drawCarModel(shader, base, Vec3(0.15f, 0.35f, 0.85f), lightsOn, 0, wheelAngle, braking);
}