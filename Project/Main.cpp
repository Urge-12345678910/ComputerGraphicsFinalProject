#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <cmath>
#include <algorithm>
#include "Math.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "Game.h"
#include "RoadSystem.h"
#include "UI.h"

static int  gWidth = 1280, gHeight = 720;
static bool gLight = true;       // F
static bool gFog = true;         // G
static int  gTimeMode = 0;       // N
static bool gHelp = false;       // H / F1
static bool gFree = false;       // C
static float gSunAz = 0.6f;      // Q/E

static Game       gGame;
static Camera     gCamera;
static RoadSystem gRoad;

static bool   gMouseInit = false;
static double gLastX = 0, gLastY = 0;

static void setFree(GLFWwindow* win, bool f)
{
    gFree = f;
    gMouseInit = false;
    if (f) gCamera.enterFree(); else gCamera.exitFree();
    glfwSetInputMode(win, GLFW_CURSOR, f ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

static void framebufferSizeCallback(GLFWwindow*, int w, int h)
{
    gWidth = w; gHeight = (h > 0) ? h : 1;
    glViewport(0, 0, w, h);
}

static void cursorCallback(GLFWwindow*, double x, double y)
{
    if (!gFree) return;
    if (!gMouseInit) { gLastX = x; gLastY = y; gMouseInit = true; return; }
    gCamera.freeLook((float)(x - gLastX), (float)(y - gLastY));
    gLastX = x; gLastY = y;
}

static void keyCallback(GLFWwindow* win, int key, int, int action, int)
{
    if (action != GLFW_PRESS) return;
    switch (key)
    {
    case GLFW_KEY_F:      gLight = !gLight; break;
    case GLFW_KEY_G:      gFog = !gFog; break;
    case GLFW_KEY_N:      gTimeMode = (gTimeMode + 1) % 3; break;
    case GLFW_KEY_H:
    case GLFW_KEY_F1:     gHelp = !gHelp; break;
    case GLFW_KEY_P:      gGame.togglePause(); break;
    case GLFW_KEY_C:      setFree(win, !gFree); break;
    case GLFW_KEY_R:      setFree(win, false); gGame.start(); break;
    case GLFW_KEY_ENTER:
    case GLFW_KEY_KP_ENTER:
        if (gGame.state == GameState::Menu) gGame.start();
        break;
    case GLFW_KEY_A: if (!gFree && gGame.state == GameState::Playing) gGame.car.changeLane(-1); break;
    case GLFW_KEY_D: if (!gFree && gGame.state == GameState::Playing) gGame.car.changeLane(+1); break;
    case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(win, true); break;
    }
}

static Vec3 tri(Vec3 night, Vec3 dusk, Vec3 day, float df)
{
    if (df < 0.35f) return mixv(night, dusk, df / 0.35f);
    return mixv(dusk, day, (df - 0.35f) / 0.65f);
}
static float tri1(float night, float dusk, float day, float df)
{
    if (df < 0.35f) return mixf(night, dusk, df / 0.35f);
    return mixf(dusk, day, (df - 0.35f) / 0.65f);
}

static int run(GLFWwindow* window)
{
    Shader shader("Shaders/vertex.vert", "Shaders/fragment.frag");
    if (!shader.isValid()) { std::cout << "Press Enter to exit...\n"; std::cin.get(); return -1; }
    if (!UI::init()) { std::cout << "UI shader failed. Press Enter to exit...\n"; std::cin.get(); return -1; }
    Mesh::init();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    float dayFactor = 0.35f;
    float fps = 60.0f, titleTimer = 0.0f;
    double last = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        double now = glfwGetTime();
        float rawDt = (float)(now - last);
        last = now;
        float dt = std::min(rawDt, 0.05f);
        if (rawDt > 0.0f) fps = fps * 0.95f + (1.0f / rawDt) * 0.05f;

        bool W = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
        bool S = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
        bool A = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
        bool D = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) gSunAz -= dt;
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) gSunAz += dt;

        if (gFree)
        {
            float f = (W ? 1.0f : 0.0f) - (S ? 1.0f : 0.0f);
            float r = (D ? 1.0f : 0.0f) - (A ? 1.0f : 0.0f);
            float u = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ? 1.0f : 0.0f)
                - (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ? 1.0f : 0.0f);
            bool fast = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
            gCamera.freeMove(dt, f, r, u, fast);
        }
        else
        {
            gGame.update(dt, W, S);
            gCamera.follow(gGame.car.pos, gGame.car.speed, dt);
        }

        const float targets[3] = { 0.35f, 1.0f, 0.0f };
        dayFactor += (targets[gTimeMode] - dayFactor) * std::min(1.0f, 2.0f * dt);
        float el = radians(tri1(35.0f, 12.0f, 60.0f, dayFactor));
        Vec3 sunDir(std::cos(el) * std::sin(gSunAz), std::sin(el), std::cos(el) * std::cos(gSunAz));
        Vec3 sunColor = tri(Vec3(0.15f, 0.17f, 0.30f), Vec3(1.0f, 0.55f, 0.28f), Vec3(1.0f, 0.96f, 0.88f), dayFactor);
        Vec3 ambient = tri(Vec3(0.07f, 0.08f, 0.16f), Vec3(0.28f, 0.24f, 0.30f), Vec3(0.38f, 0.40f, 0.45f), dayFactor);
        Vec3 sky = tri(Vec3(0.02f, 0.03f, 0.08f), Vec3(0.85f, 0.50f, 0.35f), Vec3(0.53f, 0.81f, 0.92f), dayFactor);
        bool headlights = dayFactor < 0.6f;
        bool night = dayFactor < 0.55f;

        glClearColor(sky.x, sky.y, sky.z, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();
        Mat4 view = gCamera.view();
        Mat4 proj = gCamera.projection((float)gWidth / (float)gHeight);
        shader.setMat4("view", view.ptr());
        shader.setMat4("projection", proj.ptr());
        shader.setVec3("viewPos", gCamera.position.x, gCamera.position.y, gCamera.position.z);

        shader.setBool("lightEnabled", gLight);
        shader.setVec3("sunDir", sunDir.x, sunDir.y, sunDir.z);
        shader.setVec3("sunColor", sunColor.x, sunColor.y, sunColor.z);
        shader.setVec3("ambientColor", ambient.x, ambient.y, ambient.z);

        const Vec3& cp = gGame.car.pos;
        shader.setBool("headlightsOn", headlights);
        shader.setVec3("spotPos[0]", cp.x - 0.6f, 0.65f, cp.z - 2.0f);
        shader.setVec3("spotPos[1]", cp.x + 0.6f, 0.65f, cp.z - 2.0f);
        Vec3 sd = normalize(Vec3(0.0f, -0.12f, -1.0f));
        shader.setVec3("spotDir", sd.x, sd.y, sd.z);
        shader.setFloat("spotInner", std::cos(radians(15.0f)));
        shader.setFloat("spotOuter", std::cos(radians(32.0f)));

        shader.setBool("fogEnabled", gFog);
        shader.setVec3("fogColor", sky.x, sky.y, sky.z);
        shader.setFloat("fogDensity", 0.008f);

        gRoad.draw(shader, cp.z, gGame.time, night);
        gGame.traffic.draw(shader, night);
        gGame.car.draw(shader, headlights);

        // HUD
        HudInfo hud;
        hud.state = (int)gGame.state;
        hud.speedKmh = gGame.car.speedKmh();
        hud.limit = gGame.currentLimit();
        hud.overLimit = gGame.isSpeeding();
        hud.penalties = gGame.penalties;
        hud.maxPenalties = Game::MAX_PENALTIES;
        hud.score = gGame.score();
        hud.time = gGame.time;
        hud.fps = fps;
        hud.light = gLight;
        hud.fog = gFog;
        const char* todNames[3] = { "DUSK", "DAY", "NIGHT" };
        hud.tod = todNames[gTimeMode];
        hud.freeCam = gFree;
        hud.help = gHelp;
        hud.message = gGame.message;
        hud.messageTimer = gGame.messageTimer;
        hud.messageWarning = gGame.messageIsWarning;
        hud.lightState = gGame.nextLight(hud.lightDist);

        UI::begin(gWidth, gHeight);
        UI::drawHUD(hud);
        UI::end();

        titleTimer += rawDt;
        if (titleTimer > 0.25f)
        {
            titleTimer = 0.0f;
            char title[200];
            std::snprintf(title, sizeof(title),
                "PDD Driver | Speed: %d km/h | Penalties: %d/%d | Score: %d | Light: %s (F) | Fog: %s (G)",
                (int)hud.speedKmh, hud.penalties, hud.maxPenalties, hud.score,
                gLight ? "ON" : "OFF", gFog ? "ON" : "OFF");
            glfwSetWindowTitle(window, title);
        }

        glfwSwapBuffers(window);
    }

    UI::cleanup();
    Mesh::cleanup();
    return 0;
}

int main()
{
    std::srand((unsigned)std::time(nullptr));

    if (!glfwInit()) { std::cerr << "GLFW init failed\n"; return -1; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(gWidth, gHeight, "PDD Driver", nullptr, nullptr);
    if (!window) { std::cerr << "Window creation failed\n"; glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "GLAD init failed\n";
        glfwTerminate();
        return -1;
    }
    glfwGetFramebufferSize(window, &gWidth, &gHeight);
    if (gHeight < 1) gHeight = 1;
    glViewport(0, 0, gWidth, gHeight);

    int result = run(window);   

    glfwTerminate();
    return result;
}