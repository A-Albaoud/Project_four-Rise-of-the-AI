#include "LevelA.h"

LevelA::LevelA()                                      : Scene { {0.0f}, nullptr   } {}
LevelA::LevelA(Vector2 origin, const char *bgHexCode) : Scene { origin, bgHexCode } {}

LevelA::~LevelA() { shutdown(); }

void LevelA::initialise()
{
   mGameState.nextSceneID = -1;

   mGameState.bgm = LoadMusicStream("assets/game/YTDown.com_YouTube_PAC-MAN-Medley-PAC-MAN-Super-Smash-Bros-_Media_8rJ0i7suRDk_007_128k.mp3");
   SetMusicVolume(mGameState.bgm, 0.13f);
   PlayMusicStream(mGameState.bgm);

   mGameState.jumpSound = LoadSound("assets/game/eat_fruit.wav");
   mGameState.StompSound = LoadSound("assets/game/eat_ghost.wav");
   mGameState.DeathSound = LoadSound("assets/game/death_1.wav");

   mGameState.playerDying = false;
   mGameState.deathTimer = 3.0f;
   /*
      ----------- MAP -----------
   */
   mGameState.map = new Map(
      LEVEL_WIDTH, LEVEL_HEIGHT,   // map grid cols & rows
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
      {mOrigin.x - 800.0f, mOrigin.y + 200.0f}, // position
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
   mGameState.player->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
   mGameState.player->hasShield = false;
   // ENEMIES

   mGameState.enemyCount = 1;
   mGameState.enemies = new Entity*[mGameState.enemyCount];

   mGameState.enemies[0] = new Entity(
      { mOrigin.x - 300.0f, mOrigin.y - 200.0f },
      { 63.0f, 63.0f },
      "assets/game/Clyde_single.png",
      ENEMY
   );

   mGameState.enemies[0]->setAIType(WANDERER);
   mGameState.enemies[0]->setAIState(WALKING);
   mGameState.enemies[0]->setColliderDimensions({40.0f, 40.0f});
   mGameState.enemies[0]->setAcceleration({0.0f, ACCELERATION_OF_GRAVITY});
   mGameState.enemies[0]->setDirection(LEFT);

   //key
   mGameState.goal = new Entity(
   { mOrigin.x + 860.0f, mOrigin.y + 100.0f },
   { 64.0f, 64.0f },
   "assets/game/key.png",
   BLOCK
);

mGameState.goal->setColliderDimensions({40.0f, 60.0f});

//shield

mGameState.shieldPickup = new Entity(
   { mOrigin.x - 500.0f, mOrigin.y + 50.0f },
   { 40.0f, 40.0f },
   "assets/game/Galaxian.png",
   BLOCK
);

mGameState.shieldPickup->setColliderDimensions({30.0f, 30.0f});

}

void LevelA::update(float deltaTime)
{
   UpdateMusicStream(mGameState.bgm);

   mGameState.player->update(
      deltaTime,      // delta time / fixed timestep
      nullptr,        // player
      mGameState.map, // map
      nullptr, // collidable entities
      0               // col. entity count
   );

   mGameState.enemies[0]->update(
    deltaTime,
    mGameState.player,
    mGameState.map,
    nullptr,
    0
   );


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

if (mGameState.playerDying)
{
   mGameState.deathTimer -= deltaTime;

   if (mGameState.deathTimer <= 0.0f)
   {
      if (Scene::lives <= 0) mGameState.nextSceneID = 3;
      else                   mGameState.nextSceneID = 1;
   }

   return;
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

      mGameState.playerDying = true;
      mGameState.deathTimer = 0.5f;

      return;
   }
}

   if (mGameState.goal != nullptr && mGameState.player->overlaps(mGameState.goal))
   {
      mGameState.nextSceneID = 2;
      return;
   }
   //if (mGameState.player->getPosition().y > 800.0f) mGameState.nextSceneID = 2;
}

void LevelA::render()
{
   ClearBackground(ColorFromHex(mBGColourHexCode));

   mGameState.map->render();
   mGameState.goal->render();
   mGameState.enemies[0]->render();
   mGameState.player->render();

   if (mGameState.shieldPickup != nullptr && mGameState.shieldPickup->isActive())
      mGameState.shieldPickup->render();

}

void LevelA::shutdown()
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