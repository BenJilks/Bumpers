#pragma once

#include "gameobject/scene.hpp"
#include "gameobject/forward.hpp"
#include "engine/forward.hpp"
#include <memory>

namespace Game
{

    class DebugScene : public Object::Scene
    {
    public:
        DebugScene();
        ~DebugScene();

        virtual bool init() final;
        virtual void on_render(float delta) final;
        virtual void on_resize(int width, int height) final;

    private:
        Object::GameObject *make_camera();
        void make_test_object();

        std::unique_ptr<Object::World> m_world { nullptr };
        std::shared_ptr<Engine::Renderer> m_renderer { nullptr };

    };

}
