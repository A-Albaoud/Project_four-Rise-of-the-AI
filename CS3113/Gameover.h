#include "Scene.h"

#ifndef GAMEOVER
#define GAMEOVER

class Gameover : public Scene
{
public:
    ~Gameover();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};


#endif