#pragma once
#include <string>
#include "PlayerCar.h"
#include "Traffic.h"
#include "RoadSystem.h"

enum class GameState { Menu, Playing, Paused, GameOver };

class Game
{
public:
    enum { MAX_PENALTIES = 10 };

    GameState state = GameState::Menu;
    PlayerCar car;
    Traffic   traffic;

    float time = 0.0f;
    float distance = 0.0f;
    int   penalties = 0;
    std::string message;
    float messageTimer = 0.0f;
    bool  messageIsWarning = false;

    void start();                 
    void togglePause();
    void update(float dt, bool throttle, bool brake);

    int  score() const;
    int  currentLimit() const;
    bool isSpeeding() const;
    int  nextLight(float& dist) const;   

private:
    float overTimer = 0.0f;
    float collisionCooldown = 0.0f;
    float prevFront = 0.0f;
    void addPenalty(int n, const std::string& msg);
};