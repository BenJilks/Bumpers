#include "engine/display.hpp"
#include "engine/logger.hpp"
#include "game/debug_scene.hpp"
using namespace Engine;
using namespace Object;
using namespace Game;
using namespace glm;

int main(int, char**)
{
    if (!Display::open("Roller-coaster", 800, 600))
        return 1;

    Display::set_scene(std::make_unique<DebugScene>());
    Display::start_main_loop();
    return 0;
}
