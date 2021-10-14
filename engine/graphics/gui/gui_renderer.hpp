#pragma once

#include "gameobject/forward.hpp"
#include "engine/graphics/renderer/renderer.hpp"

namespace Engine
{

    class GuiRenderer : public Renderer
    {
    public:
		GuiRenderer(std::shared_ptr<Shader> shader)
            : Renderer(shader)
        {
        }

        virtual void on_world_updated(Object::GameObject&) final;

    protected:
		virtual glm::mat4 projection_matrix(int width, int height) final;
		virtual void on_start_frame() final;
        virtual void on_render() final;

    };

}
