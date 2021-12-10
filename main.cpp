#include "engine/display.hpp"
#include "engine/logger.hpp"
#include "game/bumper_cars_scene.hpp"
#include "gameobject/world.hpp"
#include <iostream>
#include <glm/gtx/string_cast.hpp>
using namespace Engine;
using namespace Object;
using namespace Game;

int main(int, char**)
{
    if (!Display::open("Roller-coaster", 800 * 3, 600 * 3))
        return 1;

    Display::set_scene(std::make_unique<BumberCarsScene>());
    Display::start_main_loop();
    return 0;
}

