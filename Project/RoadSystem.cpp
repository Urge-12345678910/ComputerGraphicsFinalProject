#include "RoadSystem.h"
#include "Mesh.h"
#include "UI.h"
#include <cmath>
#include <algorithm>

namespace
{
    unsigned hash32(int a, int b)
    {
        unsigned h = (unsigned)a * 73856093u ^ (unsigned)b * 19349663u;
        h ^= h >> 13; h *= 0x5bd1e995u; h ^= h >> 15;
        return h;
    }
    float rnd01(unsigned h) { return (h & 0xFFFFu) / 65535.0f; }

    const Vec3 ASPHALT(0.22f, 0.22f, 0.24f);
    const Vec3 GRASS(0.18f, 0.50f, 0.20f);
    const Vec3 WHITE(0.92f, 0.92f, 0.90f);
    const Vec3 GRAY(0.45f, 0.45f, 0.48f);
    const Vec3 BUILD[5] = { Vec3(0.62f,0.55f,0.50f), Vec3(0.55f,0.58f,0.65f), Vec3(0.70f,0.66f,0.58f),
                            Vec3(0.50f,0.45f,0.45f), Vec3(0.60f,0.62f,0.55f) };
    const int LIMITS[5] = { 60, 40, 80, 50, 70 };
}

int RoadSystem::lightState(int k, float t)
{
    float ph = std::fmod(t + k * 4.3f + 3.0f, 14.0f);   
    if (ph < 6.0f) return 0;
    if (ph < 8.0f) return 1;
    return 2;
}

int RoadSystem::nextIntersection(float z)
{
    int k = (int)std::ceil((-z - 162.0f) / BLOCK);
    return k < 0 ? 0 : k;
}

int RoadSystem::speedLimitAt(float z)
{
    int k = (int)std::floor((-z - 20.0f) / BLOCK);
    if (k < 0) k = 0;
    return LIMITS[k % 5];
}

bool RoadSystem::nearIntersection(float z, float margin)
{
    int k = (int)std::floor((-z - 170.0f) / BLOCK + 0.5f);
    if (k < 0) return false;
    return std::fabs(z - intersectionZ(k)) < margin;
}

void RoadSystem::draw(const Shader& s, float viewZ, float t, bool night) const
{
    const float AHEAD = 260.0f, BEHIND = 40.0f;
    const float zMin = viewZ - AHEAD, zMax = viewZ + BEHIND;
    const float zMid = (zMin + zMax) * 0.5f, len = zMax - zMin + 20.0f;

    Mesh::drawBox(s, translate(Vec3(0, -0.1f, zMid)) * scale(Vec3(600, 0.2f, len)), GRASS);
    Mesh::drawBox(s, translate(Vec3(0, 0.0f, zMid)) * scale(Vec3(11, 0.2f, len)), ASPHALT);
    Mesh::drawBox(s, translate(Vec3(-5.1f, 0.105f, zMid)) * scale(Vec3(0.15f, 0.02f, len)), WHITE);
    Mesh::drawBox(s, translate(Vec3(5.1f, 0.105f, zMid)) * scale(Vec3(0.15f, 0.02f, len)), WHITE);
    for (float z = std::floor(zMin / 9.0f) * 9.0f; z < zMax; z += 9.0f)
    {
        Mesh::drawBox(s, translate(Vec3(-1.75f, 0.105f, z)) * scale(Vec3(0.15f, 0.02f, 3.0f)), WHITE);
        Mesh::drawBox(s, translate(Vec3(1.75f, 0.105f, z)) * scale(Vec3(0.15f, 0.02f, 3.0f)), WHITE);
    }

    int kLo = (int)std::ceil((-zMax - 170.0f) / BLOCK);
    int kHi = (int)std::floor((-zMin - 170.0f) / BLOCK);
    if (kLo < 0) kLo = 0;
    for (int k = kLo; k <= kHi; k++)
    {
        float zc = intersectionZ(k);
        Mesh::drawBox(s, translate(Vec3(0, 0.02f, zc)) * scale(Vec3(500, 0.2f, 12)), ASPHALT);
        for (int i = 0; i < 8; i++)
            Mesh::drawBox(s, translate(Vec3(-4.9f + i * 1.4f, 0.105f, zc + 6.8f)) * scale(Vec3(0.7f, 0.02f, 1.0f)), WHITE);
        Mesh::drawBox(s, translate(Vec3(0, 0.105f, zc + 8.0f)) * scale(Vec3(10.4f, 0.02f, 0.4f)), WHITE);

        float pz = zc + 9.2f;
        Mesh::drawBox(s, translate(Vec3(7.2f, 2.6f, pz)) * scale(Vec3(0.25f, 5.2f, 0.25f)), GRAY);
        Mesh::drawBox(s, translate(Vec3(4.7f, 5.3f, pz)) * scale(Vec3(5.2f, 0.2f, 0.2f)), GRAY);
        Mesh::drawBox(s, translate(Vec3(2.2f, 4.9f, pz)) * scale(Vec3(0.8f, 2.0f, 0.5f)), Vec3(0.08f, 0.08f, 0.08f));

        int st = lightState(k, t);
        const Vec3 col[3] = { Vec3(0.1f, 1.0f, 0.2f), Vec3(1.0f, 0.8f, 0.1f), Vec3(1.0f, 0.1f, 0.1f) };
        const float ly[3] = { 4.25f, 4.9f, 5.55f };
        for (int i = 0; i < 3; i++)
        {
            bool on = (i == st);
            Mesh::drawBox(s, translate(Vec3(2.2f, ly[i], pz + 0.28f)) * scale(Vec3(0.4f, 0.4f, 0.1f)),
                on ? col[i] : col[i] * 0.2f, on ? 1.0f : 0.0f);
        }
    }

    int sLo = (int)std::ceil((-zMax - 20.0f) / BLOCK);
    int sHi = (int)std::floor((-zMin - 20.0f) / BLOCK);
    if (sLo < 0) sLo = 0;
    for (int k = sLo; k <= sHi; k++)
    {
        float zs = signZ(k);
        const float sx = 7.0f;
        Mesh::drawBox(s, translate(Vec3(sx, 1.4f, zs)) * scale(Vec3(0.12f, 2.8f, 0.12f)), GRAY);
        Mesh::drawBox(s, translate(Vec3(sx, 3.4f, zs)) * scale(Vec3(1.7f, 1.7f, 0.08f)), Vec3(0.85f, 0.1f, 0.1f));
        Mesh::drawBox(s, translate(Vec3(sx, 3.4f, zs + 0.03f)) * scale(Vec3(1.4f, 1.4f, 0.08f)), Vec3(0.95f, 0.95f, 0.95f));

        int lim = LIMITS[k % 5];
        char digits[2] = { (char)('0' + lim / 10), (char)('0' + lim % 10) };
        const float p = 0.1f;
        for (int d = 0; d < 2; d++)
        {
            const unsigned char* g = UI::glyph(digits[d]);
            float x0 = sx - 0.55f + d * 6 * p;
            for (int r = 0; r < 7; r++)
            {
                int c = 0;
                while (c < 5)
                {
                    if (g[r] & (1 << (4 - c)))
                    {
                        int c0 = c;
                        while (c < 5 && (g[r] & (1 << (4 - c)))) c++;
                        float cx = x0 + (c0 + (c - c0) * 0.5f) * p;
                        Mesh::drawBox(s, translate(Vec3(cx, 3.75f - (r + 0.5f) * p, zs + 0.07f)) *
                            scale(Vec3((c - c0) * p, p, 0.04f)), Vec3(0.05f, 0.05f, 0.05f));
                    }
                    else c++;
                }
            }
        }
    }

    int bLo = (int)std::floor(zMin / 24.0f), bHi = (int)std::ceil(zMax / 24.0f);
    for (int i = bLo; i <= bHi; i++)
    {
        float zb = i * 24.0f + 12.0f;
        if (nearIntersection(zb, 17.0f)) continue;
        for (int side = -1; side <= 1; side += 2)
        {
            unsigned h = hash32(i, side);
            float w = 8.0f + rnd01(h) * 6.0f;
            float hgt = 8.0f + rnd01(h >> 4) * 26.0f;
            float xc = side * (10.5f + w * 0.5f);
            Mesh::drawBox(s, translate(Vec3(xc, hgt * 0.5f, zb)) * scale(Vec3(w, hgt, 16.0f)), BUILD[(h >> 16) % 5]);
            for (int j = 0; j < 4; j++)
            {
                float wy = 3.0f + j * 5.0f;
                if (wy > hgt - 1.5f) break;
                bool lit = ((h >> (8 + j)) & 1u) != 0;
                Vec3 wc = night ? (lit ? Vec3(1.0f, 0.80f, 0.40f) : Vec3(0.08f, 0.09f, 0.14f)) : Vec3(0.45f, 0.62f, 0.78f);
                Mesh::drawBox(s, translate(Vec3(side * 10.45f, wy, zb)) * scale(Vec3(0.12f, 1.6f, 13.0f)),
                    wc, (night && lit) ? 1.0f : 0.0f);
            }
        }
    }

    int tLo = (int)std::floor(zMin / 18.0f), tHi = (int)std::ceil(zMax / 18.0f);
    for (int i = tLo; i <= tHi; i++)
    {
        unsigned h = hash32(i, 77);
        float zt = i * 18.0f + rnd01(h) * 6.0f;
        if (nearIntersection(zt, 12.0f)) continue;
        for (int side = -1; side <= 1; side += 2)
        {
            float x = side * (7.8f + rnd01(h >> 8) * 0.8f);
            Mesh::drawBox(s, translate(Vec3(x, 1.0f, zt)) * scale(Vec3(0.4f, 2.0f, 0.4f)), Vec3(0.35f, 0.22f, 0.10f));
            Mesh::drawBox(s, translate(Vec3(x, 3.2f, zt)) * scale(Vec3(2.2f, 2.4f, 2.2f)), Vec3(0.10f, 0.40f, 0.12f));
        }
    }

    int lLo = (int)std::floor(zMin / 40.0f), lHi = (int)std::ceil(zMax / 40.0f);
    for (int i = lLo; i <= lHi; i++)
    {
        float zl = i * 40.0f + 8.0f;
        if (nearIntersection(zl, 14.0f)) continue;
        for (int side = -1; side <= 1; side += 2)
        {
            Mesh::drawBox(s, translate(Vec3(side * 6.4f, 3.0f, zl)) * scale(Vec3(0.15f, 6.0f, 0.15f)), GRAY);
            Mesh::drawBox(s, translate(Vec3(side * 5.6f, 6.05f, zl)) * scale(Vec3(1.6f, 0.2f, 0.4f)),
                night ? Vec3(1.0f, 0.9f, 0.6f) : GRAY, night ? 1.0f : 0.0f);
        }
    }
}