#pragma once

#include "gameobject/component.hpp"
#include <glm/glm.hpp>

namespace Object::Gui
{

    class Control : public ComponentBase<Control>
    {
        friend ComponentBase<Control>;

    public:
        inline const glm::vec2 &anchor() const { return m_anchor; }

    private:
        Control(const Control&) = default;

        Control(glm::vec2 anchor)
            : m_anchor(anchor)
        {
        }

        glm::vec2 m_anchor;

    };

}
