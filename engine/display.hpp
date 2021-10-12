#pragma once

#include "gameobject/forward.hpp"
#include <string_view>
#include <functional>
#include <memory>

namespace Engine::Display 
{

    bool open(std::string_view title, int width, int height);
    void set_scene(std::unique_ptr<Object::Scene> scene);
    void start_main_loop();

    int width();
    int height();

}
