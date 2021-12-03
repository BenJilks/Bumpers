#include "player_controller.hpp"
#include "gameobject/gameobject.hpp"
#include "engine/input.hpp"
#include <GL/freeglut.h>
#include <cassert>
using namespace Game;
using namespace Engine;

void PlayerController::init(Object::GameObject &gameobject)
{
    m_engine = gameobject.first<CarEngine>();
}

void PlayerController::update(Object::GameObject&, float delta)
{
    assert (m_engine);

    m_engine->set_action(CarEngine::Action::Forward, Input::is_key_down(GLUT_KEY_UP));
    m_engine->set_action(CarEngine::Action::Reverse, Input::is_key_down(GLUT_KEY_DOWN));
    m_engine->set_action(CarEngine::Action::TurnLeft, Input::is_key_down(GLUT_KEY_LEFT));
    m_engine->set_action(CarEngine::Action::TurnRight, Input::is_key_down(GLUT_KEY_RIGHT));
}

