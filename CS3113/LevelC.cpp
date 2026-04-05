#include "LevelC.h"

LevelC::LevelC()                                      : Scene { {0.0f}, nullptr   } {}
LevelC::LevelC(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelC::~LevelC() { shutdown(); }

void LevelC::initialise()
{

   mGameState.nextSceneID = -1;
   mGameState.bgm = LoadMusicStream("assets/game/YTDown.com_YouTube_DuckTales-Music-NES-The-Moon-Theme_Media_KF32DRg9opA_007_128k.mp3");
   SetMusicVolume(mGameState.bgm, 0.33f);
   PlayMusicStream(mGameState.bgm);

   mGameState.jumpSound = LoadSound("assets/game/eat_fruit.wav");
   mGameState.StompSound = LoadSound("assets/game/eat_ghost.wav");
   mGameState.DeathSound = LoadSound("assets/game/death_1.wav");

   /*
      ----------- MAP -----------
   */
   mGameState.map = new Map(
      LEVELC_WIDTH, LEVELC_HEIGHT,   // map grid cols & rows
      (unsigned int *) mLevelData, // grid data
      "assets/game/mapsheet.png",   // texture filepath
      TILE_DIMENSION,              // tile size
      6, 1,                        // texture cols & rows
      mOrigin                      // in-game origin
   );

   /*
      ----------- PROTAGONIST -----------
   */
   std::map<Direction, std::vector<int>> playerAnimationAtlas = {
    {RIGHT,  { 0, 1, 2, 1 }},
    {LEFT, { 3, 4, 2, 3 }}
   };

   float sizeRatio  = 48.0f / 64.0f;

   // Assets from @see https://sscary.itch.io/the-adventurer-female
   mGameState.player = new Entity(
      {mOrigin.x - 1300.0f, mOrigin.y + 1300.0f}, // position
      {70.0f, 70.0f},             // scale
      "assets/game/pacman.png",                   // texture file address
      ATLAS,                                    // single image or atlas?
      { 2, 3 },                                 // atlas dimensions
      playerAnimationAtlas,                    // actual atlas
      PLAYER                                    // entity type
   );

   mGameState.player->setJumpingPower(550.0f);
   mGameState.player->setColliderDimensions({
   mGameState.player->getScale().x / 3.5f,
   mGameState.player->getScale().y / 3.0f
   });
   mGameState.player->setAcceleration({0.0f, 300.0f});
   mGameState.player->hasShield = false;


   mGameState.enemyCount = 2;
   mGameState.enemies = new Entity*[mGameState.enemyCount];

   mGameState.enemies[0] = new Entity(
      { mOrigin.x - 150.0f, mOrigin.y + 300.0f },
      { 63.0f, 63.0f },
      "assets/game/Clyde_single.png",
      ENEMY
   );

   mGameState.enemies[0]->setAIType(FLYER);
   mGameState.enemies[0]->setAIState(WALKING);
   mGameState.enemies[0]->setColliderDimensions({40.0f, 40.0f});
   mGameState.enemies[0]->setAcceleration({0.0f, 0.0f});
   mGameState.enemies[0]->setDirection(UP);
   mGameState.enemies[0]->setStartY(mGameState.enemies[0]->getPosition().y);

   mGameState.enemies[1] = new Entity(
      { mOrigin.x + 220.0f, mOrigin.y + 150.0f },
      { 63.0f, 63.0f },
      "assets/game/blinky_single.png",
      ENEMY
   );

   mGameState.enemies[1]->setAIType(FOLLOWER);
   mGameState.enemies[1]->setAIState(WALKING);
   mGameState.enemies[1]->setColliderDimensions({40.0f, 40.0f});
   mGameState.enemies[1]->setAcceleration({0.0f, 650.0f});
   mGameState.enemies[1]->setDirection(RIGHT);

   mGameState.goal = new Entity(
   { mOrigin.x + 70.0f, mOrigin.y  - 1900.0f },
   { 64.0f, 64.0f },
   "assets/game/key.png",
   BLOCK
   );

   mGameState.goal->setColliderDimensions({40.0f, 60.0f});

   mGameState.shieldPickup = new Entity(
      { mOrigin.x + 150.0f, mOrigin.y + 300.0f },
      { 40.0f, 40.0f },
      "assets/game/Galaxian.png",
      BLOCK
   );

   mGameState.shieldPickup->setColliderDimensions({30.0f, 30.0f});   
}

void LevelC::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   mGameState.player->update(
      deltaTime,
      nullptr,
      mGameState.map,
      nullptr,
      0
   );

   for (int i = 0; i < mGameState.enemyCount; i++)
   {
      mGameState.enemies[i]->update(
         deltaTime,
         mGameState.player,
         mGameState.map,
         nullptr,
         0
      );
   }

   if (mGameState.shieldPickup != nullptr && mGameState.shieldPickup->isActive())
   {
      if (!mGameState.player->hasShield)
      {
         if (mGameState.player->overlaps(mGameState.shieldPickup))
         {
            mGameState.player->hasShield = true;
         }
      }
      else
      {
         Vector2 current = mGameState.shieldPickup->getPosition();

         Vector2 target = {
            mGameState.player->getPosition().x + 45.0f,
            mGameState.player->getPosition().y - 35.0f
         };

         float lerpAmount = 0.12f;

         Vector2 followPosition = {
            current.x + (target.x - current.x) * lerpAmount,
            current.y + (target.y - current.y) * lerpAmount
         };

         mGameState.shieldPickup->setPosition(followPosition);
      }
   }

   for (int i = 0; i < mGameState.enemyCount; i++)
   {
      Entity *enemy = mGameState.enemies[i];

      if (!enemy->isActive()) continue;
      if (!mGameState.player->overlaps(enemy)) continue;

      if (mGameState.player->hasShield)
      {
         mGameState.player->hasShield = false;

         if (mGameState.shieldPickup != nullptr)
            mGameState.shieldPickup->deactivate();

         enemy->deactivate();
         PlaySound(mGameState.StompSound);
         return;
      }
      else
      {
         PlaySound(mGameState.DeathSound);

         Scene::lives--;

         if (Scene::lives <= 0) mGameState.nextSceneID = 3;
         else                   mGameState.nextSceneID = 1;

         return;
      }
   }

   if (mGameState.goal != nullptr && mGameState.player->overlaps(mGameState.goal))
   {
      mGameState.nextSceneID = 5;   // back to menu, or change if you want a win scene
      return;
   }

   if (mGameState.player->getPosition().y > mOrigin.y + 1700.0f)
   {
      Scene::lives--;

      if (Scene::lives <= 0) mGameState.nextSceneID = 3;
      else                   mGameState.nextSceneID = 2;

      return;
   }
}

void LevelC::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.map->render();

   if (mGameState.goal != nullptr)
      mGameState.goal->render();

   if (mGameState.shieldPickup != nullptr && mGameState.shieldPickup->isActive())
      mGameState.shieldPickup->render();

   for (int i = 0; i < mGameState.enemyCount; i++)
   {
      if (mGameState.enemies[i]->isActive())
         mGameState.enemies[i]->render();
   }

   mGameState.player->render();
}

void LevelC::shutdown()
{
   delete mGameState.player;
   delete mGameState.map;

   for (int i = 0; i < mGameState.enemyCount; i++)
   {
      delete mGameState.enemies[i];
   }
   delete[] mGameState.enemies;

   delete mGameState.goal;
   delete mGameState.shieldPickup;

   UnloadMusicStream(mGameState.bgm);
   UnloadSound(mGameState.jumpSound);
   UnloadSound(mGameState.StompSound);
   UnloadSound(mGameState.DeathSound);
}