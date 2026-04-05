#include "Gameover.h"
#include "raylib.h"
#include "cs3113.h"

#define GAMEOVER_BG ColorFromHex("#200000")

void Gameover::initialise()
{
    mGameState.player = nullptr;
    mGameState.map = nullptr;
    mGameState.enemies = nullptr;
    mGameState.enemyCount = 0;
    mGameState.nextSceneID = -1;
}

void Gameover::update(float deltaTime)
{
    if (IsKeyPressed(KEY_R))
    {
        Scene::lives = Scene::STARTING_LIVES;
        mGameState.nextSceneID = 0;
    }
}

void Gameover::render()
{
    ClearBackground(GAMEOVER_BG);
    DrawText("GAME OVER", 300, 180, 50, WHITE);
    DrawText("Press R to Restart", 240, 260, 30, WHITE);
}

void Gameover::shutdown() {}

Gameover::~Gameover() {}