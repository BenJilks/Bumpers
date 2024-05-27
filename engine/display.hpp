/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "gameobject/forward.hpp"
#include <memory>
#include <string_view>

namespace Engine::Display {

bool open(std::string_view title, int width, int height);
void set_scene(std::unique_ptr<Object::Scene> scene);
void start_main_loop();

int width();
int height();

}
