#include "Game.h"
#include <cmath>
#include <algorithm>

void Game::start()
{
    car.reset();
    traffic.reset();
    time = 0.0f;
    distance = 0.0f;
    penalties = 0;
    overTimer = 0.0f;
    collisionCooldown = 0.0f;
    prevFront = car.pos.z - 2.0f;
    message = "OBEY TRAFFIC LIGHTS AND SPEED LIMIT SIGNS";
    messageTimer = 6.0f;
    messageIsWarning = false;
    state = GameState::Playing;
}

void Game::togglePause()
{
    if (state == GameState::Playing) state = GameState::Paused;
    else if (state == GameState::Paused) state = GameState::Playing;
}

void Game::addPenalty(int n, const std::string& msg)
{
    penalties += n;
    message = msg;
    messageTimer = 3.5f;
    messageIsWarning = true;
}

int Game::score() const
{
    int s = (int)(distance / 10.0f) - penalties * 20;
    return s < 0 ? 0 : s;
}

int Game::currentLimit() const { return RoadSystem::speedLimitAt(car.pos.z); }

bool Game::isSpeeding() const { return car.speedKmh() > currentLimit() + 5.0f; }

int Game::nextLight(float& dist) const
{
    float front = car.pos.z - 2.0f;
    int k = RoadSystem::nextIntersection(front);
    dist = front - RoadSystem::stopLineZ(k);
    if (dist < 0.0f) { k++; dist = front - RoadSystem::stopLineZ(k); }
    return RoadSystem::lightState(k, time);
}

void Game::update(float dt, bool throttle, bool brake)
{
    if (messageTimer > 0.0f) messageTimer -= dt;
    if (state != GameState::Playing) return;

    time += dt;
    collisionCooldown -= dt;

    car.update(dt, throttle, brake);
    traffic.update(dt, time, car);
    distance += car.speed * dt;

    for (const auto& c : traffic.cars)
    {
        float dx = std::fabs(car.pos.x - c.pos.x);
        float dz = car.pos.z - c.pos.z;
        if (dx < 1.75f && std::fabs(dz) < 4.0f)
        {
            car.pos.z = c.pos.z + (dz >= -2.0f ? 4.05f : -4.05f);
            car.speed = std::min(car.speed, c.speed * 0.5f);
            if (collisionCooldown <= 0.0f)
            {
                addPenalty(2, "COLLISION! KEEP A SAFE DISTANCE");
                collisionCooldown = 1.5f;
            }
        }
    }

    float front = car.pos.z - 2.0f;
    int k = RoadSystem::nextIntersection(prevFront);
    float stopZ = RoadSystem::stopLineZ(k);
    if (prevFront > stopZ && front <= stopZ)
    {
        int st = RoadSystem::lightState(k, time);
        if (st == 2) addPenalty(1, "RED LIGHT! STOP BEHIND THE STOP LINE");
        else if (st == 1)
        {
            message = "YELLOW LIGHT: STOP IF IT IS SAFE";
            messageTimer = 2.5f;
            messageIsWarning = false;
        }
    }
    prevFront = front;

    if (isSpeeding())
    {
        overTimer += dt;
        if (overTimer >= 2.0f)
        {
            addPenalty(1, "SPEEDING! LIMIT IS " + std::to_string(currentLimit()) + " KM/H");
            overTimer = -3.0f;
        }
    }
    else
    {
        overTimer = (overTimer > 0.0f) ? 0.0f : std::min(0.0f, overTimer + dt);
    }

    if (penalties >= MAX_PENALTIES) state = GameState::GameOver;
}