#pragma once

#include "gameobject/gameobject.hpp"
#include "gameobject/scene.hpp"
#include "gameobject/forward.hpp"
#include "engine/physics/collision_resolver_2d.hpp"
#include "engine/forward.hpp"
#include <memory>
#include <glm/glm.hpp>

namespace Game
{

    class BumberCarsScene : public Object::Scene
    {
    public:
        BumberCarsScene()
        {
        }

        ~BumberCarsScene()
        {
        }

        virtual bool init() final;
        virtual void on_render(float delta) final;
        virtual void on_resize(int width, int height) final;

    private:
        Object::GameObject *make_cameras(Object::GameObject &player);
        Object::GameObject *make_bumber_car();
        Object::GameObject *make_arena();
        Object::GameObject *make_debug_cube();
        void make_ai(Object::GameObject &car_template, glm::vec3 position, glm::vec3 color);
        void make_sky_box(std::shared_ptr<Engine::Texture> sky_box_texture);

        std::unique_ptr<Object::World> m_world { nullptr };
        std::unique_ptr<Engine::CollisionResolver2D> m_collision_resolver { nullptr };

        std::shared_ptr<Engine::StandardRenderer> m_renderer { nullptr };
        std::shared_ptr<Engine::SkyBoxRenderer> m_sky_box_renderer { nullptr };
        std::shared_ptr<Engine::RenderTexture> m_view { nullptr };
        std::shared_ptr<Engine::BloomRenderer> m_bloom_renderer { nullptr };

        Object::GameObject *m_in_car_camera { nullptr };
        Object::GameObject *m_look_at_camera { nullptr };
        Object::GameObject *m_free_camera { nullptr };

    };

}
