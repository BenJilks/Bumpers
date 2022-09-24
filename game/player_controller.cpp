/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "player_controller.hpp"
#include "gameobject/gameobject.hpp"
#include "engine/input.hpp"
#include <cassert>
using namespace Game;
using namespace Engine;

void PlayerController::init(Object::GameObject &game_object)
{
    m_engine = game_object.first<CarEngine>();
}

void PlayerController::update(Object::GameObject&, float delta)
{
    assert (m_engine);

    m_engine->set_action(CarEngine::Action::Forward, Input::is_key_down(Input::Key::Up));
    m_engine->set_action(CarEngine::Action::Reverse, Input::is_key_down(Input::Key::Down));
    m_engine->set_action(CarEngine::Action::TurnLeft, Input::is_key_down(Input::Key::Left));
    m_engine->set_action(CarEngine::Action::TurnRight, Input::is_key_down(Input::Key::Right));
}

