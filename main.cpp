/**
* Author: Abdulrahman Albaoud
* Assignment: Rise of the AI
* Date due: 2025-11-08, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/
#include "CS3113/LevelB.h"
#include "CS3113/Menu.h"
#include "CS3113/Gameover.h"
#include "CS3113/LevelC.h"
#include "CS3113/Winscreen.h"

// Global Constants
constexpr int SCREEN_WIDTH     = 1000,
              SCREEN_HEIGHT    = 600,
              FPS              = 120,
              NUMBER_OF_LEVELS = 6;

constexpr Vector2 ORIGIN      = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
            
constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus   = RUNNING;
float gPreviousTicks   = 0.0f,
      gTimeAccumulator = 0.0f;

Camera2D gCamera = { 0 };

Scene *gCurrentScene = nullptr;
std::vector<Scene*> gLevels = {};

Winscreen *gWinscreen = nullptr;
Gameover *gGameover  = nullptr;
Menu     *gMenu      = nullptr;
LevelA   *gLevelA    = nullptr;
LevelB   *gLevelB    = nullptr;
LevelC   *gLevelC    = nullptr;

// Function Declarations
void switchToScene(Scene *scene);
void initialise();
void processInput();
void update();
void render();
void shutdown();

void switchToScene(Scene *scene)
{
    if (gCurrentScene != nullptr)
    {
        gCurrentScene->shutdown();
    }

    gCurrentScene = scene;
    gCurrentScene->initialise();

    if (gCurrentScene->getState().player != nullptr)
    {
        gCamera.target = gCurrentScene->getState().player->getPosition();
    }
    else
    {
        gCamera.target = ORIGIN;
    }
}

void loseLife()
{
    Scene::lives--;

    if (Scene::lives <= 0)
    {
        switchToScene(gLevels[3]);
    }
    else
    {
        switchToScene(gLevels[1]);
    }
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Scenes");
    InitAudioDevice();
    
    gMenu     = new Menu();
    gLevelA   = new LevelA(ORIGIN, "#C0897E");
    gLevelB   = new LevelB(ORIGIN, "#011627");
    gGameover = new Gameover();
    gLevelC   = new LevelC(ORIGIN, "#011627");
    gWinscreen = new Winscreen();

    gLevels.push_back(gMenu); //0
    gLevels.push_back(gLevelA); //1
    gLevels.push_back(gLevelB); //2
    gLevels.push_back(gGameover); //3
    gLevels.push_back(gLevelC); //4
    gLevels.push_back(gWinscreen); //5

    switchToScene(gLevels[0]);

    gCamera.offset   = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom     = 1.0f;

    SetTargetFPS(FPS);
}

void processInput() 
{
    Entity *player = gCurrentScene->getState().player;

    if (player != nullptr)
    {
        player->resetMovement();

        if      (IsKeyDown(KEY_A)) player->moveLeft();
        else if (IsKeyDown(KEY_D)) player->moveRight();

        if (IsKeyPressed(KEY_W) && player->isCollidingBottom())
        {
            player->jump();
            PlaySound(gCurrentScene->getState().jumpSound);
        }

        if (GetLength(player->getMovement()) > 1.0f)
            player->normaliseMovement();
    }

    if (IsKeyPressed(KEY_Q) || WindowShouldClose()){
        gAppStatus = TERMINATED;
    }

    if (IsKeyPressed(KEY_ONE))
    {
        switchToScene(gLevels[1]);   // LevelA
        return;
    }

    if (IsKeyPressed(KEY_TWO))
    {
        switchToScene(gLevels[2]);   // LevelB
        return;
    }
    if (IsKeyPressed(KEY_THREE))
    {
        switchToScene(gLevels[4]);   // LevelB
        return;
    }
}

    

void update() 
{

    Entity *player = gCurrentScene->getState().player;
    float ticks = (float) GetTime();
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;

        if (player != nullptr)
        {
            Vector2 currentPlayerPosition = {
                player->getPosition().x,
                player->getPosition().y
            };
            panCamera(&gCamera, &currentPlayerPosition);
        }
    }

    if (player != nullptr && player->getPosition().y > SCREEN_HEIGHT + 1200)
    {
        loseLife();
        return;
    }
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCamera);

    gCurrentScene->render();

    EndMode2D();
    DrawText(TextFormat("Lives: %d", Scene::lives), 20, 20, 30, WHITE);
    EndDrawing();
}

void shutdown() 
{
    delete gMenu;
    delete gLevelA;
    delete gLevelB;
    delete gGameover;
    delete gLevelC;
    delete gWinscreen;

    for (int i = 0; i < NUMBER_OF_LEVELS; i++) gLevels[i] = nullptr;

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();

        if (gCurrentScene->getState().nextSceneID >=0)
        {
            int id = gCurrentScene->getState().nextSceneID;
            switchToScene(gLevels[id]);
        }

        render();
    }

    shutdown();

    return 0;
}