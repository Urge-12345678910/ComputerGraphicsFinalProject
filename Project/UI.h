#pragma once
#include <string>

struct HudInfo
{
    int   state = 0;
    float speedKmh = 0;
    int   limit = 60;
    bool  overLimit = false;
    int   penalties = 0, maxPenalties = 10;
    int   score = 0;
    float time = 0;
    float fps = 0;
    bool  light = true, fog = false;
    std::string tod;
    bool  freeCam = false, help = false;
    std::string message;
    float messageTimer = 0;
    bool  messageWarning = false;
    int   lightState = 0;
    float lightDist = 0;
};

namespace UI
{
    bool  init();
    void  cleanup();
    void  begin(int w, int h);
    void  end();
    void  rect(float x, float y, float w, float h, float r, float g, float b, float a = 1.0f);
    void  text(float x, float y, float scale, const std::string& str, float r, float g, float b, float a = 1.0f);
    float textWidth(const std::string& str, float scale);
    const unsigned char* glyph(char c);
    void  drawHUD(const HudInfo& h);
}