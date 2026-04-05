#include "Winscreen.h"
#include "raylib.h"
#include "cs3113.h"

#define WINSCREEN_BG ColorFromHex("#babd09ff")

void Winscreen::initialise()
{
    mGameState.player = nullptr;
    mGameState.map = nullptr;
    mGameState.enemies = nullptr;
    mGameState.enemyCount = 0;
    mGameState.nextSceneID = -1;
}

void Winscreen::update(float deltaTime)
{
    if (IsKeyPressed(KEY_R))
    {
        Scene::lives = Scene::STARTING_LIVES;
        mGameState.nextSceneID = 0;
    }
}

void Winscreen::render()
{
    ClearBackground(WINSCREEN_BG);
    DrawText("YOU WIN GOODJOB", 300, 180, 50, WHITE);
    DrawText("Press R to Restart", 240, 260, 30, WHITE);
}

void Winscreen::shutdown() {}

Winscreen::~Winscreen() {}