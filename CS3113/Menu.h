#include "Scene.h"

#ifndef MENU
#define MENU

class Menu : public Scene
{
public:
    ~Menu();

    void initialise() override;
    void update(float deltaTime) override;
    void render() override;
    void shutdown() override;
};


#endif