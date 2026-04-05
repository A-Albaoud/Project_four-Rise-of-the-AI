#include "Scene.h"

#ifndef WINSCREEN
#define WINSCREEN

class Winscreen : public Scene
{
public:
    ~Winscreen();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};


#endif