/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "engine/forward.hpp"
#include "engine/physics/collision_resolver_2d.hpp"
#include "gameobject/forward.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/scene.hpp"
#include <glm/glm.hpp>
#include <memory>

namespace Game {

class BumperCarsScene final : public Object::Scene {
public:
    BumperCarsScene();
    ~BumperCarsScene() final;

    bool init() final;
    void on_render(float delta) final;
    void on_resize(int width, int height) final;

private:
    Object::GameObject* make_cameras(Object::GameObject& player);
    Object::GameObject* make_bumper_car(Engine::AssetRepository const&);
    Object::GameObject* make_arena(Engine::AssetRepository const&);
    void make_ai(Object::GameObject& car_template, glm::vec3 position, glm::vec3 color);
    void make_sky_box(std::shared_ptr<Engine::Texture> sky_box_texture);

    std::unique_ptr<Object::World> m_world { nullptr };
    std::unique_ptr<Engine::CollisionResolver2D> m_collision_resolver { nullptr };

    std::shared_ptr<Engine::StandardRenderer> m_renderer { nullptr };
    std::shared_ptr<Engine::SkyBoxRenderer> m_sky_box_renderer { nullptr };
    std::shared_ptr<Engine::RenderTexture> m_view { nullptr };
    std::shared_ptr<Engine::BloomRenderer> m_bloom_renderer { nullptr };

    Object::GameObject* m_in_car_camera { nullptr };
    Object::GameObject* m_look_at_camera { nullptr };
    Object::GameObject* m_free_camera { nullptr };
    bool m_finished_loading { false };
};

}
