#pragma once
#include <vector>
#include "Math.h"
#include "Shader.h"
#include "PlayerCar.h"

struct TrafficCar
{
    int  style = 0;
    bool braking = false;
    Vec3  pos;
    float speed = 0.0f;
    float cruise = 12.0f;
    int   lane = 0;
    Vec3  color;
};

class Traffic
{
public:
    std::vector<TrafficCar> cars;

    void reset();
    void update(float dt, float t, const PlayerCar& player);
    void draw(const Shader& s, bool lightsOn) const;

private:
    float spawnTimer = 1.0f;
    void trySpawn(float playerZ, float minD, float maxD);
};