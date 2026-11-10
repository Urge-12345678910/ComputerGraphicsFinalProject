#pragma once
#include "Math.h"
#include "Shader.h"

class RoadSystem
{
public:
    static constexpr float BLOCK = 300.0f;

    static float intersectionZ(int k) { return -(170.0f + BLOCK * k); }
    static float stopLineZ(int k) { return intersectionZ(k) + 8.0f; }
    static float signZ(int k) { return -(20.0f + BLOCK * k); }

    static int  lightState(int k, float t);
    static int  nextIntersection(float z);
    static int  speedLimitAt(float z);
    static bool nearIntersection(float z, float margin);

    void draw(const Shader& s, float viewZ, float t, bool night) const;
};