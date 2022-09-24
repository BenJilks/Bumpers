#include "engine/display.hpp"
#include "game/bumper_cars_scene.hpp"
#include "gameobject/world.hpp"
using namespace Engine;
using namespace Object;
using namespace Game;

int main(int, char**)
{
    if (!Display::open("Roller-coaster", 800 * 2, 600 * 2))
        return 1;

    Display::set_scene(std::make_unique<BumperCarsScene>());
    Display::start_main_loop();
    return 0;
}

