#pragma once

#include "gameobject/forward.hpp"
#include "engine/graphics/renderer.hpp"

namespace Engine
{

    class GuiRenderer : public Renderer
    {
    public:
		GuiRenderer(std::shared_ptr<Shader> shader, int width, int height)
			: Renderer(shader, width, height)
        {
        }

        virtual void render(const Object::GameObject&) override;
        virtual void resize_viewport(int width, int height) override;

    private:
    };

}
