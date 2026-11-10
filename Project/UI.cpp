#include <glad/glad.h>
#include "UI.h"
#include "Shader.h"
#include <vector>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <iostream>

#define TEAM_NAME "TEAM: Zero_Kik"

namespace
{
    struct Glyph { char c; unsigned char r[7]; };
    const Glyph FONT[] = {
        {' ',{0,0,0,0,0,0,0}},
        {'0',{0x0E,0x11,0x13,0x15,0x19,0x11,0x0E}}, {'1',{0x04,0x0C,0x04,0x04,0x04,0x04,0x0E}},
        {'2',{0x0E,0x11,0x01,0x02,0x04,0x08,0x1F}}, {'3',{0x1F,0x02,0x04,0x02,0x01,0x11,0x0E}},
        {'4',{0x02,0x06,0x0A,0x12,0x1F,0x02,0x02}}, {'5',{0x1F,0x10,0x1E,0x01,0x01,0x11,0x0E}},
        {'6',{0x06,0x08,0x10,0x1E,0x11,0x11,0x0E}}, {'7',{0x1F,0x01,0x02,0x04,0x08,0x08,0x08}},
        {'8',{0x0E,0x11,0x11,0x0E,0x11,0x11,0x0E}}, {'9',{0x0E,0x11,0x11,0x0F,0x01,0x02,0x0C}},
        {'A',{0x0E,0x11,0x11,0x1F,0x11,0x11,0x11}}, {'B',{0x1E,0x11,0x11,0x1E,0x11,0x11,0x1E}},
        {'C',{0x0E,0x11,0x10,0x10,0x10,0x11,0x0E}}, {'D',{0x1C,0x12,0x11,0x11,0x11,0x12,0x1C}},
        {'E',{0x1F,0x10,0x10,0x1E,0x10,0x10,0x1F}}, {'F',{0x1F,0x10,0x10,0x1E,0x10,0x10,0x10}},
        {'G',{0x0E,0x11,0x10,0x17,0x11,0x11,0x0F}}, {'H',{0x11,0x11,0x11,0x1F,0x11,0x11,0x11}},
        {'I',{0x0E,0x04,0x04,0x04,0x04,0x04,0x0E}}, {'J',{0x07,0x02,0x02,0x02,0x02,0x12,0x0C}},
        {'K',{0x11,0x12,0x14,0x18,0x14,0x12,0x11}}, {'L',{0x10,0x10,0x10,0x10,0x10,0x10,0x1F}},
        {'M',{0x11,0x1B,0x15,0x15,0x11,0x11,0x11}}, {'N',{0x11,0x11,0x19,0x15,0x13,0x11,0x11}},
        {'O',{0x0E,0x11,0x11,0x11,0x11,0x11,0x0E}}, {'P',{0x1E,0x11,0x11,0x1E,0x10,0x10,0x10}},
        {'Q',{0x0E,0x11,0x11,0x11,0x15,0x12,0x0D}}, {'R',{0x1E,0x11,0x11,0x1E,0x14,0x12,0x11}},
        {'S',{0x0F,0x10,0x10,0x0E,0x01,0x01,0x1E}}, {'T',{0x1F,0x04,0x04,0x04,0x04,0x04,0x04}},
        {'U',{0x11,0x11,0x11,0x11,0x11,0x11,0x0E}}, {'V',{0x11,0x11,0x11,0x11,0x11,0x0A,0x04}},
        {'W',{0x11,0x11,0x11,0x15,0x15,0x15,0x0A}}, {'X',{0x11,0x11,0x0A,0x04,0x0A,0x11,0x11}},
        {'Y',{0x11,0x11,0x11,0x0A,0x04,0x04,0x04}}, {'Z',{0x1F,0x01,0x02,0x04,0x08,0x10,0x1F}},
        {'.',{0,0,0,0,0,0x0C,0x0C}}, {',',{0,0,0,0,0x0C,0x04,0x08}},
        {':',{0,0x0C,0x0C,0,0x0C,0x0C,0}}, {'!',{0x04,0x04,0x04,0x04,0x04,0,0x04}},
        {'?',{0x0E,0x11,0x01,0x02,0x04,0,0x04}}, {'-',{0,0,0,0x1F,0,0,0}},
        {'/',{0x01,0x01,0x02,0x04,0x08,0x10,0x10}}, {'(',{0x02,0x04,0x08,0x08,0x08,0x04,0x02}},
        {')',{0x08,0x04,0x02,0x02,0x02,0x04,0x08}}, {'%',{0x18,0x19,0x02,0x04,0x08,0x13,0x03}},
        {'+',{0,0x04,0x04,0x1F,0x04,0x04,0}}, {'=',{0,0,0x1F,0,0x1F,0,0}},
        {'|',{0x04,0x04,0x04,0x04,0x04,0x04,0x04}}, {'_',{0,0,0,0,0,0,0x1F}},
    };

    unsigned char gTable[128][7];
    Shader* gShader = nullptr;
    unsigned int  gVao = 0, gVbo = 0;
    std::vector<float> gVerts;
    int gW = 1, gH = 1;
}

bool UI::init()
{
    std::memset(gTable, 0, sizeof(gTable));
    for (const auto& g : FONT) std::memcpy(gTable[(unsigned char)g.c], g.r, 7);

    gShader = new Shader("Shaders/ui.vert", "Shaders/ui.frag");
    if (!gShader->isValid()) return false;

    glGenVertexArrays(1, &gVao);
    glGenBuffers(1, &gVbo);
    glBindVertexArray(gVao);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    return true;
}

void UI::cleanup()
{
    delete gShader;
    gShader = nullptr;
    glDeleteVertexArrays(1, &gVao);
    glDeleteBuffers(1, &gVbo);
}

const unsigned char* UI::glyph(char c)
{
    if (c >= 'a' && c <= 'z') c = (char)(c - 32);
    unsigned char i = (unsigned char)c;
    if (i > 127) i = 0;
    return gTable[i];
}

void UI::begin(int w, int h) { gW = w; gH = h; gVerts.clear(); }

void UI::end()
{
    if (gVerts.empty()) return;
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    gShader->use();
    gShader->setFloat("screenW", (float)gW);
    gShader->setFloat("screenH", (float)gH);
    glBindVertexArray(gVao);
    glBindBuffer(GL_ARRAY_BUFFER, gVbo);
    glBufferData(GL_ARRAY_BUFFER, gVerts.size() * sizeof(float), gVerts.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, (int)(gVerts.size() / 6));

    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    gVerts.clear();
}

void UI::rect(float x, float y, float w, float h, float r, float g, float b, float a)
{
    float x1 = x + w, y1 = y + h;
    const float q[6][2] = { {x,y},{x1,y},{x1,y1},{x,y},{x1,y1},{x,y1} };
    for (int i = 0; i < 6; i++)
    {
        gVerts.push_back(q[i][0]); gVerts.push_back(q[i][1]);
        gVerts.push_back(r); gVerts.push_back(g); gVerts.push_back(b); gVerts.push_back(a);
    }
}

float UI::textWidth(const std::string& str, float scale)
{
    return str.empty() ? 0.0f : (float)str.size() * 6.0f * scale - scale;
}

void UI::text(float x, float y, float s, const std::string& str, float r, float g, float b, float a)
{
    float cx = x;
    for (char ch : str)
    {
        const unsigned char* gl = glyph(ch);
        for (int row = 0; row < 7; row++)
        {
            int c = 0;
            while (c < 5)
            {
                if (gl[row] & (1 << (4 - c)))
                {
                    int c0 = c;
                    while (c < 5 && (gl[row] & (1 << (4 - c)))) c++;
                    rect(cx + c0 * s, y + row * s, (c - c0) * s, s, r, g, b, a);
                }
                else c++;
            }
        }
        cx += 6.0f * s;
    }
}

void UI::drawHUD(const HudInfo& h)
{
    const float W = (float)gW, H = (float)gH;
    const float sc = std::max(2.0f, std::floor(H / 360.0f));
    const float lh = 10.0f * sc;
    char b[128];

    auto center = [&](float y, float s, const std::string& str, float r, float g, float bl, float a = 1.0f) {
        text((W - textWidth(str, s)) * 0.5f, y, s, str, r, g, bl, a);
        };

    float pw = 24 * 6 * sc + 16;
    rect(10, 10, pw, 6 * lh + 14, 0, 0, 0, 0.5f);
    float x = 18, y = 17;
    std::snprintf(b, sizeof b, "SPEED: %d KM/H", (int)h.speedKmh);
    text(x, y, sc, b, 1.0f, h.overLimit ? 0.3f : 1.0f, h.overLimit ? 0.3f : 1.0f); y += lh;
    std::snprintf(b, sizeof b, "LIMIT: %d KM/H", h.limit);
    text(x, y, sc, b, 1.0f, 0.85f, 0.3f); y += lh;
    std::snprintf(b, sizeof b, "PENALTIES: %d/%d", h.penalties, h.maxPenalties);
    text(x, y, sc, b, 1.0f, h.penalties > 0 ? 0.4f : 1.0f, h.penalties > 0 ? 0.4f : 1.0f); y += lh;
    std::snprintf(b, sizeof b, "SCORE: %d", h.score);
    text(x, y, sc, b, 1, 1, 1); y += lh;
    std::snprintf(b, sizeof b, "TIME: %02d:%02d", (int)h.time / 60, (int)h.time % 60);
    text(x, y, sc, b, 1, 1, 1); y += lh;
    {
        const char* names[3] = { "GREEN", "YELLOW", "RED" };
        const float cols[3][3] = { {0.2f,1.0f,0.3f}, {1.0f,0.85f,0.1f}, {1.0f,0.2f,0.2f} };
        int st = std::max(0, std::min(2, h.lightState));
        rect(x, y, 7 * sc, 7 * sc, cols[st][0], cols[st][1], cols[st][2]);
        std::snprintf(b, sizeof b, "LIGHT %d M: %s", (int)h.lightDist, names[st]);
        text(x + 10 * sc, y, sc, b, 1, 1, 1);
    }

    float pw2 = 20 * 6 * sc + 16, px2 = W - pw2 - 10;
    rect(px2, 10, pw2, 6 * lh + 14, 0, 0, 0, 0.5f);
    x = px2 + 8; y = 17;
    std::snprintf(b, sizeof b, "FPS: %d", (int)h.fps);
    text(x, y, sc, b, 1, 1, 1); y += lh;
    std::snprintf(b, sizeof b, "LIGHTING: %s (F)", h.light ? "ON" : "OFF");
    text(x, y, sc, b, h.light ? 0.4f : 1.0f, h.light ? 1.0f : 0.4f, 0.4f); y += lh;
    std::snprintf(b, sizeof b, "FOG: %s (G)", h.fog ? "ON" : "OFF");
    text(x, y, sc, b, h.fog ? 0.4f : 1.0f, h.fog ? 1.0f : 0.4f, 0.4f); y += lh;
    text(x, y, sc, "TIME: " + h.tod + " (N)", 1, 1, 1); y += lh;
    text(x, y, sc, std::string("CAMERA: ") + (h.freeCam ? "FREE" : "GAME") + " (C)", 1, 1, 1); y += lh;
    text(x, y, sc, "SUN: Q/E", 1, 1, 1);

    float bh = lh + 10;
    rect(0, H - bh, W, bh, 0, 0, 0, 0.6f);
    text(10, H - bh + 5, sc, "PDD DRIVER - 3D TRAFFIC RULES TRAINER | " TEAM_NAME, 0.9f, 0.9f, 0.9f);
    std::string rt = "H: HELP  P: PAUSE  R: RESTART";
    text(W - textWidth(rt, sc) - 10, H - bh + 5, sc, rt, 0.9f, 0.9f, 0.9f);

    if (h.state == 1 && h.messageTimer > 0.0f && !h.message.empty())
    {
        float ms = sc + 1.0f;
        float tw = textWidth(h.message, ms);
        float mx = (W - tw) * 0.5f, my = H * 0.16f;
        float a = std::min(1.0f, h.messageTimer);
        if (h.messageWarning) rect(mx - 14, my - 10, tw + 28, 7 * ms + 20, 0.7f, 0.05f, 0.05f, 0.8f * a);
        else                  rect(mx - 14, my - 10, tw + 28, 7 * ms + 20, 0.1f, 0.3f, 0.6f, 0.8f * a);
        text(mx, my, ms, h.message, 1, 1, 1, a);
    }

    if (h.freeCam)
        center(H * 0.08f, sc, "FREE CAMERA - GAME PAUSED - C: BACK TO GAME", 1.0f, 0.9f, 0.3f);

    if (h.state == 0)
    {
        rect(0, 0, W, H, 0, 0, 0, 0.55f);
        center(H * 0.14f, sc * 4, "PDD DRIVER", 1.0f, 0.85f, 0.2f);
        center(H * 0.14f + 9 * sc * 4, sc * 1.5f, "LEARN TRAFFIC RULES WHILE YOU DRIVE", 1, 1, 1);
        float yy = H * 0.42f;
        const char* rules[] = {
            "RED LIGHT = STOP BEHIND THE STOP LINE",
            "OBEY SPEED LIMIT SIGNS",
            "KEEP A SAFE DISTANCE FROM OTHER CARS",
            "10 PENALTIES = GAME OVER",
            "W/S: GAS / BRAKE     A/D: CHANGE LANE" };
        for (const char* r : rules) { center(yy, sc * 1.5f, r, 0.9f, 0.9f, 0.9f); yy += 12 * sc * 1.5f; }
        center(yy + 10 * sc, sc * 2.0f, "PRESS ENTER TO START", 0.4f, 1.0f, 0.4f);
    }
    else if (h.state == 2)
    {
        rect(0, 0, W, H, 0, 0, 0, 0.5f);
        center(H * 0.38f, sc * 4, "PAUSED", 1, 1, 1);
        center(H * 0.38f + 9 * sc * 4, sc * 2, "PRESS P TO CONTINUE", 0.9f, 0.9f, 0.9f);
    }
    else if (h.state == 3)
    {
        rect(0, 0, W, H, 0, 0, 0, 0.65f);
        center(H * 0.30f, sc * 4, "GAME OVER", 1.0f, 0.3f, 0.3f);
        center(H * 0.30f + 9 * sc * 4, sc * 2, "TOO MANY VIOLATIONS", 1, 1, 1);
        std::snprintf(b, sizeof b, "SCORE: %d   TIME: %02d:%02d", h.score, (int)h.time / 60, (int)h.time % 60);
        center(H * 0.30f + 9 * sc * 4 + 14 * sc * 2, sc * 2, b, 1.0f, 0.85f, 0.3f);
        center(H * 0.30f + 9 * sc * 4 + 28 * sc * 2, sc * 2, "PRESS R TO RESTART", 0.4f, 1.0f, 0.4f);
    }

    if (h.help)
    {
        const char* lines[] = {
            "CONTROLS",
            "",
            "W / S      GAS / BRAKE",
            "A / D      CHANGE LANE",
            "P          PAUSE",
            "R          RESTART",
            "F          LIGHTING ON/OFF (MAIN TOGGLE)",
            "G          FOG ON/OFF",
            "N          DUSK / DAY / NIGHT",
            "Q / E      ROTATE THE SUN",
            "C          FREE CAMERA (WASD, MOUSE,",
            "           SPACE/CTRL UP/DOWN, SHIFT FAST)",
            "H / F1     THIS HELP",
            "ESC        QUIT" };
        int n = (int)(sizeof(lines) / sizeof(lines[0]));
        float s2 = sc;
        float tw = 44 * 6 * s2;
        float ph = n * lh + 24;
        float px = (W - tw) * 0.5f - 14, py = (H - ph) * 0.5f;
        rect(px, py, tw + 28, ph, 0.05f, 0.05f, 0.1f, 0.92f);
        for (int i = 0; i < n; i++)
            text(px + 14, py + 12 + i * lh, s2, lines[i], i == 0 ? 1.0f : 0.9f, i == 0 ? 0.85f : 0.9f, i == 0 ? 0.2f : 0.9f);
    }
}