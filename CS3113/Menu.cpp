#include "Menu.h"
#include "raylib.h"
#include "cs3113.h"

#define MENU_BG ColorFromHex("#C98C84")

void Menu::initialise()
{
    mGameState.player = nullptr;
    mGameState.map = nullptr;
    mGameState.enemies = nullptr;
    mGameState.enemyCount = 0;
    mGameState.nextSceneID = -1;
}

void Menu::update(float deltaTime)
{
    if (IsKeyPressed(KEY_ENTER))
    {
        Scene::lives = Scene::STARTING_LIVES;
        mGameState.nextSceneID = 1;
    }
}

void Menu::render()
{
    ClearBackground(MENU_BG);

    DrawText("Pac man to the moon", 140, 150, 40, WHITE);
    DrawText("Press Enter to Start", 145, 240, 30, WHITE);
}

void Menu::shutdown()
{
}

Menu::~Menu() {}