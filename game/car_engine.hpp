#pragma once

#include "config.hpp"
#include "gameobject/component.hpp"
#include "gameobject/forward.hpp"
#include <array>

namespace Game
{

    class CarEngine : public Object::ComponentBase<CarEngine>
    {
        friend Object::ComponentBase<CarEngine>;

    public:
        virtual void init(Object::GameObject&) override;
        virtual void update(Object::GameObject&, float delta) override;

        enum class Action
        {
            Forward,
            Reverse,
            TurnLeft,
            TurnRight,
            
            Count,
        };

        inline void set_action(Action action, bool value) { m_action_enabled[(size_t)action] = value; }

    private:
        CarEngine(const CarEngine&) = default;
        CarEngine() = default;

        inline bool is_action_enabled(Action action) const { return m_action_enabled[(size_t)action]; }

        float m_wheel_direction { 0 };
        std::array<bool, (size_t)Action::Count> m_action_enabled { };

        Object::PhysicsBody2D *m_physics_body { nullptr };
        Object::Transform *m_transform { nullptr };
        Object::Collider2D *m_collider { nullptr };

    };

}

