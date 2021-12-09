#include "ai.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include "engine/input.hpp"
#include "engine/physics/collision_shape_utils_2d.hpp"
#include "glm/geometric.hpp"
#include "glm/gtx/string_cast.hpp"
#include <cassert>
#include <cstdlib>
using namespace Game;
using namespace Object;
using namespace Engine;
using namespace glm;

void AI::init(Object::GameObject &gameobject)
{
    m_engine = gameobject.first<CarEngine>();
    m_transform = gameobject.first<Transform>();
}

void AI::update(Object::GameObject&, float delta)
{
    assert (m_engine);
    assert (m_transform);
    m_engine->set_action(CarEngine::Action::Forward, true);
    m_timer_until_next_action -= delta;

    auto distance_from_center = glm::length(vec_3to2(m_transform->position()) * vec2(1, 0.5f));
    if (distance_from_center >= 15)
    {
        auto forward = vec_3to2(m_transform->left());
        auto to_center = glm::normalize(vec_3to2(m_transform->position()));
        auto dot = glm::dot(forward, to_center);
        m_engine->set_action(CarEngine::Action::TurnLeft, dot > 0);
        m_engine->set_action(CarEngine::Action::TurnRight, dot < 0);
    }
    else
    {
        if (m_timer_until_next_action <= 0)
        {
            m_engine->set_action(CarEngine::Action::TurnLeft, (std::rand() % 2) == 0);
            m_engine->set_action(CarEngine::Action::TurnRight, (std::rand() % 2) == 0);
            m_timer_until_next_action = ((std::rand() % 100) / 100.0f) * 0.5f;
        }
    }
}

