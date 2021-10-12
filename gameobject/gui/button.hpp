#pragma once

#include "engine/forward.hpp"
#include "gameobject/component.hpp"
#include <functional>

namespace Object::Gui
{

    class Button : public ComponentBase<Button>
    {
        friend ComponentBase<Button>;

    public:
        virtual void init(GameObject&) override;
        virtual void update(GameObject&, float delta) override;

        std::function<void()> on_click;

    private:
        Button(const Button&) = default;

        Button(std::shared_ptr<Engine::Texture> normal, std::shared_ptr<Engine::Texture> hover)
            : m_normal(normal)
            , m_hover(hover)
        {
        }

        std::shared_ptr<Engine::Texture> m_normal;
        std::shared_ptr<Engine::Texture> m_hover;

        Transform *m_transform { nullptr };
        Control *m_control { nullptr };
        MeshRender *m_mesh_render { nullptr };
        bool m_hovered { false };

    };

}
