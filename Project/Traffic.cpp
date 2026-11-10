#include "Traffic.h"
#include "RoadSystem.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

namespace
{
    const float CAR_LEN = 4.0f;

    float followTarget(float gap, float leaderSpeed)
    {
        const float minGap = 2.5f;
        if (gap <= minGap) return std::max(0.0f, leaderSpeed - 2.0f);
        return leaderSpeed + std::sqrt(2.0f * 7.0f * (gap - minGap));
    }
}

void Traffic::reset()
{
    cars.clear();
    spawnTimer = 1.0f;
    for (int i = 0; i < 10; i++) trySpawn(0.0f, 40.0f, 260.0f);
}

void Traffic::trySpawn(float playerZ, float minD, float maxD)
{
    if (cars.size() >= 14) return;
    int lane = std::rand() % 3 - 1;
    float z = playerZ - (minD + (float)(std::rand() % (int)(maxD - minD)));
    if (RoadSystem::nearIntersection(z, 22.0f)) return;
    for (const auto& o : cars)
        if (o.lane == lane && std::fabs(o.pos.z - z) < 35.0f) return;

    static const Vec3 palette[6] = { Vec3(0.8f,0.15f,0.15f), Vec3(0.9f,0.8f,0.2f), Vec3(0.2f,0.6f,0.3f),
                                     Vec3(0.85f,0.85f,0.85f), Vec3(0.3f,0.3f,0.35f), Vec3(0.9f,0.45f,0.1f) };
    TrafficCar c;
    c.lane = lane;
    c.pos = Vec3(lane * PlayerCar::LANE_WIDTH, 0.0f, z);
    c.cruise = 8.0f + (float)(std::rand() % 11);
    c.speed = c.cruise;
    c.color = palette[std::rand() % 6];
    c.style = std::rand() % 4;
    cars.push_back(c);
}

void Traffic::update(float dt, float t, const PlayerCar& player)
{
    spawnTimer -= dt;
    if (spawnTimer <= 0.0f)
    {
        trySpawn(player.pos.z, 170.0f, 260.0f);
        spawnTimer = 0.6f + (float)(std::rand() % 100) / 100.0f;
    }

    for (auto& c : cars)
    {
        float gap = 1e9f, leaderSpeed = c.cruise;
        for (const auto& o : cars)
        {
            if (&o == &c || o.lane != c.lane || o.pos.z >= c.pos.z) continue;
            float g = c.pos.z - o.pos.z - CAR_LEN;
            if (g < gap) { gap = g; leaderSpeed = o.speed; }
        }
        if (std::fabs(player.pos.x - c.lane * PlayerCar::LANE_WIDTH) < 2.0f && player.pos.z < c.pos.z)
        {
            float g = c.pos.z - player.pos.z - CAR_LEN;
            if (g < gap) { gap = g; leaderSpeed = player.speed; }
        }

        float target = c.cruise;
        if (gap < 1e8f) target = std::min(target, followTarget(gap, leaderSpeed));

        float front = c.pos.z - 2.0f;
        int k = RoadSystem::nextIntersection(front);
        float dLine = front - RoadSystem::stopLineZ(k);
        if (dLine > 0.0f && RoadSystem::lightState(k, t) != 0)
        {
            float stopDist = c.speed * c.speed / (2.0f * 9.0f);
            if (dLine > stopDist) target = std::min(target, followTarget(dLine - 1.0f, 0.0f));
        }
        c.braking = target < c.speed - 0.3f;
        if (c.speed < target) c.speed = std::min(target, c.speed + 3.5f * dt);
        else                  c.speed = std::max(target, c.speed - 12.0f * dt);
        c.pos.z -= c.speed * dt;
    }

    for (auto& c : cars)
        for (const auto& o : cars)
        {
            if (&o == &c || o.lane != c.lane || o.pos.z >= c.pos.z) continue;
            if (c.pos.z - o.pos.z - CAR_LEN < 0.2f)
            {
                c.pos.z = o.pos.z + CAR_LEN + 0.2f;
                c.speed = std::min(c.speed, o.speed);
            }
        }

    cars.erase(std::remove_if(cars.begin(), cars.end(), [&](const TrafficCar& c) {
        return c.pos.z > player.pos.z + 50.0f || c.pos.z < player.pos.z - 400.0f;
        }), cars.end());
}

void Traffic::draw(const Shader& s, bool lightsOn) const
{
    for (const auto& c : cars)
        drawCarModel(s, translate(c.pos), c.color, lightsOn, c.style,
            std::fmod(c.pos.z / 0.35f, 6.2831853f), c.braking);
}