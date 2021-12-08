#include "debug_scene.hpp"
#include "debug_camera_movement.hpp"
#include "engine/graphics/mesh/material.hpp"
#include "game/car_engine.hpp"
#include "game/player_controller.hpp"
#include "gameobject/forward.hpp"
#include "spin.hpp"
#include "config.hpp"
#include "engine/assets/obj_loader.hpp"
#include "engine/assets/collada_loader.hpp"
#include "engine/graphics/mesh/mesh_builder.hpp"
#include "engine/graphics/texture/image_texture.hpp"
#include "engine/graphics/texture/render_texture.hpp"
#include "engine/graphics/texture/cube_map_texture.hpp"
#include "engine/graphics/shader.hpp"
#include "engine/graphics/renderer/standard_renderer.hpp"
#include "engine/graphics/renderer/sky_box_renderer.hpp"
#include "engine/graphics/renderer/blur_renderer.hpp"
#include "engine/graphics/renderer/bloom_renderer.hpp"
#include "engine/physics/collision_shape.hpp"
#include "engine/physics/collision_resolver.hpp"
#include "engine/input.hpp"
#include "gameobject/world.hpp"
#include "gameobject/camera.hpp"
#include "gameobject/transform.hpp"
#include "gameobject/mesh_render.hpp"
#include "gameobject/light.hpp"
#include "gameobject/physics/physics_body.hpp"
#include "gameobject/physics/collider.hpp"
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <limits>
#include <memory>
using namespace Engine;
using namespace Object;
using namespace Game;
using namespace glm;

DebugScene::DebugScene()
{
}

DebugScene::~DebugScene()
{
}

Object::GameObject *DebugScene::make_camera()
{
    auto &camera = m_world->add_child();
    camera.add_component<Transform>();
    camera.add_component<Camera>();
    camera.add_component<DebugCameraMovement>();
    return &camera;
}

class Circle : public ComponentBase<Circle>
{
    friend ComponentBase<Circle>;

public:
    virtual ~Circle()
    {
    }

    virtual void update(GameObject &gameobject, float delta) 
    {
        m_time += delta;

        auto *transform = gameobject.first<Transform>();
        auto x = std::sin(m_time + m_offset) * 6.0f;
        auto z = std::cos(m_time + m_offset) * 6.0f;
        transform->set_position(vec3(x, transform->position().y, z));
    }

private:
    Circle(const Circle&) = default;
    Circle(float offset)
        : m_offset(offset)
    {
    }

    float m_time { 0 };
    float m_offset { 0 };

};

GameObject *DebugScene::make_bumber_car()
{
    auto *bumber_car = ColladaLoader::from_file(*m_world, ASSETS + "/models/bumper.dae", 
        [&](Object::GameObject &object, Engine::MeshBuilder &builder, ColladaLoader::ModelMetaData meta_data)
    {
        object.add_component<MeshRender>(builder.build(), m_renderer, meta_data.material);

        auto &transform = object.add_component<Transform>();
        transform.set_position(meta_data.translation);
        transform.set_scale(meta_data.scale);
        transform.set_rotation(meta_data.rotation);
    });

    if (!bumber_car)
        return nullptr;

    auto front_collider = std::make_shared<CollisionShapeCircle>(vec2(0, 2.02552), 1.65037);
    auto body_collider = std::make_shared<CollisionShapeOBB>(vec2(0), vec2(1.71153, 1.95445));
    auto back_collider = std::make_shared<CollisionShapeCircle>(vec2(0, -1.95445), 1.65037);
    bumber_car->add_component<Transform>();
    bumber_car->add_component<PhysicsBody>(vec2(6, 4), 1, 1, 0.2f);
    bumber_car->add_component<Collider>(front_collider);
    bumber_car->add_component<Collider>(body_collider);
    bumber_car->add_component<Collider>(back_collider);
    bumber_car->add_component<CarEngine>();
    return bumber_car;
}

GameObject *DebugScene::make_arena()
{
    auto *arena = ColladaLoader::from_file(*m_world, ASSETS + "/models/arena.dae", 
        [&](Object::GameObject &object, Engine::MeshBuilder &builder, ColladaLoader::ModelMetaData meta_data)
    {
        object.add_component<MeshRender>(builder.build(), m_renderer, meta_data.material);

        auto &transform = object.add_component<Transform>();
        transform.set_position(meta_data.translation);
        transform.set_scale(meta_data.scale);
        transform.set_rotation(meta_data.rotation);
    });

    if (!arena)
        return nullptr;
    
    // auto collision_shape = std::make_shared<CollisionShapeAABB>(vec2(0, 4), vec2(10, 1));
    // arena->add_component<Transform>();
    // arena->add_component<PhysicsBody>(vec2(1), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
    // arena->add_component<Collider>(collision_shape);

    for (auto z : { -0.499666, 23.3924, 47.3231, -24.331, -48.2455 })
    {
        auto &light_a = arena->add_child();
        auto &light_a_transform = light_a.add_component<Transform>();
        light_a.add_component<Light>(vec3(1.0f, 0.8f, 0.8f));
        light_a_transform.translate(vec3(-14.0093, 12.9745, z));
        
        auto &light_b = arena->add_child();
        auto &light_b_transform = light_b.add_component<Transform>();
        light_b.add_component<Light>(vec3(1.0f, 0.8f, 0.8f));
        light_b_transform.translate(vec3(14.0093, 12.9745, z));
    }

    return arena;
}

Object::GameObject *DebugScene::make_debug_cube()
{
    auto *cube = ColladaLoader::from_file(*m_world, ASSETS + "/models/cube.dae",
        [&](Object::GameObject &object, Engine::MeshBuilder &builder, ColladaLoader::ModelMetaData meta_data)
    {
        object.add_component<MeshRender>(builder.build(), m_renderer, meta_data.material);

        auto &transform = object.add_component<Transform>();
        transform.set_position(meta_data.translation);
        transform.set_scale(meta_data.scale);
        transform.set_rotation(meta_data.rotation);
    });

    if (!cube)
        return nullptr;    
    else
        return cube;
}

void DebugScene::make_sky_box(std::shared_ptr<Texture> sky_box_texture)
{
    auto skybox_mesh = MeshBuilder()
        .add_sky_box(6)
        .build();

    auto &skybox = m_world->add_child();
    skybox.add_component<Transform>();
    skybox.add_component<MeshRender>(skybox_mesh, m_sky_box_renderer, 
        Material { .diffuse_map = sky_box_texture });
}

bool DebugScene::init()
{
    auto skybox_texture = CubeMapTexture::construct(ASSETS + "/textures/skybox/skybox");

    auto shader = Shader::construct(ASSETS + "/shaders/default.glsl");
    auto skybox_shader = Shader::construct(ASSETS + "/shaders/skybox.glsl");
    auto bloom_shader = Shader::construct(ASSETS + "/shaders/bloom.glsl");
    auto blur_shader = Shader::construct(ASSETS + "/shaders/blur.glsl");

    m_world = std::make_unique<World>();
    m_collision_resolver = std::make_unique<CollisionResolver>();
    
    m_renderer = std::make_shared<StandardRenderer>(shader, skybox_texture);
    m_sky_box_renderer = std::make_shared<SkyBoxRenderer>(skybox_shader);
    m_view = RenderTexture::construct({ m_sky_box_renderer, m_renderer });
    m_view->add_color_texture();

    m_bloom_renderer = std::make_shared<BloomRenderer>(
        bloom_shader, blur_shader,
        m_view->color_texture(0), m_view->color_texture(1));

    auto *camera = make_camera();
    if (!camera)
        return false;

    auto *arena = make_arena();
    if (!arena)
        return false;

    auto *bumber_car_template = make_bumber_car();
    if (!bumber_car_template)
        return false;

    for (int i = -2; i < 2; i++)
    {
        auto &ai = bumber_car_template->clone(*m_world);
        ai.first<Transform>()->translate(vec3(i * 5, 0, 10));
    }

    auto *player = bumber_car_template;
    player->add_component<PlayerController>();

    // make_sky_box(skybox_texture);

    m_renderer->set_camera(*camera);
    m_renderer->on_world_updated(*m_world);

    m_sky_box_renderer->set_camera(*camera);
    m_sky_box_renderer->on_world_updated(*m_world);

    m_world->init();
    return true;
}

void DebugScene::on_render(float delta)
{
    m_world->step_physics(delta);
    m_world->update(delta);
    m_collision_resolver->resolve(*m_world);

    m_view->render();
    m_bloom_renderer->pre_render();
    m_bloom_renderer->render();
}

void DebugScene::on_resize(int width, int height)
{
    m_view->resize(width, height);
    m_bloom_renderer->resize_viewport(width, height);
}

