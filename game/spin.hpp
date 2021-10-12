#pragma once

#include "gameobject/component.hpp"
#include "gameobject/forward.hpp"

namespace Game
{

    class Spin : public Object::ComponentBase<Spin>
    {
        friend Object::ComponentBase<Spin>;

    public:
        virtual void init(Object::GameObject&) override;
        virtual void update(Object::GameObject&, float delta) override;

    private:
        Spin(const Spin&) = default;
        Spin() = default;

        float m_speed { 0 };
        float m_counter { 0 };
        Object::Transform *m_transform { nullptr };

    };

}
