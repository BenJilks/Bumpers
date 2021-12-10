#include "bumper_cars_scene.hpp"
#include "car_engine.hpp"
#include "engine/assets/thread_pool.hpp"
#include "game/free_camera.hpp"
#include "game/in_car_camera.hpp"
#include "gameobject/attributes.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/physics/box_bounds_3d.hpp"
#include "player_controller.hpp"
#include "ai.hpp"
#include "engine/graphics/mesh/material.hpp"
#include "gameobject/forward.hpp"
#include "config.hpp"
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
#include "engine/physics/collision_shape_2d.hpp"
#include "engine/physics/collision_resolver_2d.hpp"
#include "engine/input.hpp"
#include "gameobject/world.hpp"
#include "gameobject/camera.hpp"
#include "gameobject/transform.hpp"
#include "gameobject/mesh_render.hpp"
#include "gameobject/light.hpp"
#include "gameobject/physics/physics_body_2d.hpp"
#include "gameobject/physics/collider_2d.hpp"
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <limits>
#include <memory>
using namespace Engine;
using namespace Object;
using namespace Game;
using namespace glm;

const std::vector<BoxBounds3D::Box> bounding_boxes =
{
    { vec3(27.2268, 2.78474, 0), vec3(1.044855, 4.06174, 32.9089) },
    { vec3(-27.6395, 2.78474, 4.52736), vec3(1.044855, 4.06174, 27.1582) },
    { vec3(1.02131, 2.78474, -45.4816), vec3(14.2001, 4.06174, 1.006829) },
    { vec3(1.02131, 2.78474, 45.4816), vec3(14.2001, 4.06174, 1.0068297) },

    { vec3(21.3328, 2.78474, 38.4206), vec3(6.01089, 4.06174, 6.88848) },
    { vec3(-20.059, 2.78474, 38.8872), vec3(6.83231, 4.06174, 6.33768) },
    { vec3(-20.059, 2.78474, -38.8989), vec3(6.83231, 4.06174, 6.33768) },
    { vec3(21.1664, 2.78474, -38.7045), vec3(6.01089, 4.06174, 6.88848) },

    { vec3(-28.867, 8.32688, 46.8004), vec3(1.4763, 8.31515, 1.4763) },
    { vec3(-28.867, 8.32688, 23.2155), vec3(1.4763, 8.31515, 1.4763) },
    { vec3(-28.867, 8.32688, -0.7390), vec3(1.4763, 8.31515, 1.4763) },
    { vec3(-28.867, 8.32688, -24.493), vec3(1.4763, 8.31515, 1.4763) },
    { vec3(-28.867, 8.32688, -48.320), vec3(1.4763, 8.31515, 1.4763) },

    { vec3(28.867, 8.32688, 46.8004), vec3(1.4763, 8.31515, 1.4763) },
    { vec3(28.867, 8.32688, 23.2155), vec3(1.4763, 8.31515, 1.4763) },
    { vec3(28.867, 8.32688, -0.7390), vec3(1.4763, 8.31515, 1.4763) },
    { vec3(28.867, 8.32688, -24.493), vec3(1.4763, 8.31515, 1.4763) },
    { vec3(28.867, 8.32688, -48.320), vec3(1.4763, 8.31515, 1.4763) },

    { vec3(0, 28.867, -0.626996), vec3(33.6523, 15.1126, 51.9021) },
};

BumberCarsScene::BumberCarsScene()
{
}

BumberCarsScene::~BumberCarsScene()
{
}

Object::GameObject *BumberCarsScene::make_cameras(GameObject &player)
{
    auto &free_camera = m_world->add_child();
    auto &free_camera_transform = free_camera.add_component<Transform>();
    free_camera.add_component<Camera>();
    free_camera.add_component<FreeCamera>();
    free_camera.add_component<BoxBounds3D>(bounding_boxes);
    free_camera_transform.translate(vec3(0, 4, 0));

    auto &in_car_camera = m_world->add_child();
    in_car_camera.add_component<Transform>();
    in_car_camera.add_component<Camera>();
    in_car_camera.add_component<InCarCamera>(&player);

    m_free_camera = &free_camera;
    m_in_car_camera = &in_car_camera;

    return &free_camera;
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

GameObject *BumberCarsScene::make_bumber_car()
{
    auto *bumber_car = ColladaLoader::from_file(*m_world, ASSETS + "/models/bumper.dae", 
        [&](Object::GameObject &object, Engine::MeshBuilder &builder, ColladaLoader::ModelMetaData meta_data)
    {
        object.add_component<MeshRender>(builder.build(), m_renderer, meta_data.material);
        object.add_component<Attributes>(meta_data.name);

        auto &transform = object.add_component<Transform>();
        transform.set_position(meta_data.translation);
        transform.set_scale(meta_data.scale);
        transform.set_rotation(meta_data.rotation);
    });

    if (!bumber_car)
        return nullptr;

    auto front_collider = std::make_shared<CollisionShapeCircle2D>(vec2(0, 2.02552), 1.65037);
    auto body_collider = std::make_shared<CollisionShapeOBB2D>(vec2(0), vec2(1.71153, 1.95445));
    auto back_collider = std::make_shared<CollisionShapeCircle2D>(vec2(0, -1.95445), 1.65037);
    bumber_car->add_component<Transform>();
    bumber_car->add_component<PhysicsBody2D>(vec2(6, 4), 1, 1, 0.2f);
    bumber_car->add_component<Collider2D>(front_collider);
    bumber_car->add_component<Collider2D>(body_collider);
    bumber_car->add_component<Collider2D>(back_collider);
    bumber_car->add_component<CarEngine>();
    return bumber_car;
}

GameObject *BumberCarsScene::make_arena()
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
    
    arena->add_component<Transform>();
    arena->add_component<PhysicsBody2D>(vec2(1), 0.5, std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());

    auto add_collider = [&](vec2 position, vec2 scale, float rotation = 0)
    {
        arena->add_component<Collider2D>(std::make_shared<CollisionShapeOBB2D>(position, scale, rotation));
    };

    add_collider(vec2(0, 46.4112), vec2(13.411, 3.18407));
    add_collider(vec2(0, -46.4112), vec2(13.411, 3.18407));
    add_collider(vec2(28.79, 0), vec2(3.51, 31.6541));
    add_collider(vec2(-28.79, 0), vec2(3.51, 31.6541));

    add_collider(vec2(21.1239, 39.0009), vec2(3.51, 31.6541), glm::radians(45.0f));
    add_collider(vec2(-21.1239, 39.0009), vec2(3.51, 31.6541), glm::radians(-45.0f));
    add_collider(vec2(-21.1239, -39.0009), vec2(3.51, 31.6541), glm::radians(45.0f));
    add_collider(vec2(21.1239, -39.0009), vec2(3.51, 31.6541), glm::radians(-45.0f));

    constexpr vec3 colors[] =
    {
        vec3(1.0f, 0.5f, 0.5f), vec3(0.5f, 1.0f, 0.5f),
        vec3(0.5f, 0.5f, 1.0f), vec3(1.0f, 1.0f, 0.5f),
        vec3(0.5f, 1.0f, 1.0f), vec3(1.0f, 0.5f, 1.0f),
        vec3(1.0f, 1.0f, 1.0f), vec3(0.8f, 0.6f, 0.4f),
        vec3(1.0f, 0.8f, 0.8f), vec3(0.5f, 1.0f, 0.5f),
    };

    constexpr auto z_offsets = std::array
    {
        -0.499666, 23.3924, 47.3231, -24.331, -48.2455,
    };

    for (int i = 0; i < z_offsets.size(); i++)
    {
#if 0
        const auto &color_a = colors[i*2+0];
        const auto &color_b = colors[i*2+1];
#else
        const auto &color_a = vec3(1.0f, 0.8f, 0.8f);
        const auto &color_b = vec3(1.0f, 0.8f, 0.8f);
#endif

        auto &light_a = arena->add_child();
        auto &light_a_transform = light_a.add_component<Transform>();
        light_a.add_component<Light>(color_a);
        light_a_transform.translate(vec3(-14.0093, 12.9745, z_offsets[i]));
        
        auto &light_b = arena->add_child();
        auto &light_b_transform = light_b.add_component<Transform>();
        light_b.add_component<Light>(color_b);
        light_b_transform.translate(vec3(14.0093, 12.9745, z_offsets[i]));
    }

    return arena;
}

Object::GameObject *BumberCarsScene::make_debug_cube()
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

void BumberCarsScene::make_sky_box(std::shared_ptr<Texture> sky_box_texture)
{
    auto skybox_mesh = MeshBuilder()
        .add_sky_box(6)
        .build();

    auto &skybox = m_world->add_child();
    skybox.add_component<Transform>();
    skybox.add_component<MeshRender>(skybox_mesh, m_sky_box_renderer, 
        Material { .diffuse_map = sky_box_texture });
}

static void set_bumper_car_color(GameObject &car, vec3 color)
{
    car.for_each([&color](GameObject &object)
    {
        auto *attributes = object.first<Attributes>();
        if (!attributes)
            return IteratorDecision::Continue;
        if (!attributes->has("Cart"))
            return IteratorDecision::Continue;

        auto *mesh_render = object.first<MeshRender>();
        assert (mesh_render);

        mesh_render->material().color = color;
        return IteratorDecision::Break;
    });
}

void BumberCarsScene::make_ai(GameObject &car_template, vec3 position, vec3 color)
{
    auto &ai = car_template.clone(*m_world);
    ai.add_component<AI>();
    ai.first<Transform>()->translate(position);
    set_bumper_car_color(ai, color);
}

bool BumberCarsScene::init()
{
    auto skybox_texture = CubeMapTexture::construct(ASSETS + "/textures/skybox/skybox");

    auto shader = Shader::construct(ASSETS + "/shaders/default.glsl");
    auto skybox_shader = Shader::construct(ASSETS + "/shaders/skybox.glsl");
    auto bloom_shader = Shader::construct(ASSETS + "/shaders/bloom.glsl");
    auto blur_shader = Shader::construct(ASSETS + "/shaders/blur.glsl");

    m_world = std::make_unique<World>();
    m_collision_resolver = std::make_unique<CollisionResolver2D>();
    
    m_renderer = std::make_shared<StandardRenderer>(shader, skybox_texture);
    m_sky_box_renderer = std::make_shared<SkyBoxRenderer>(skybox_shader);
    m_view = RenderTexture::construct({ m_sky_box_renderer, m_renderer });
    m_view->add_color_texture();

    m_bloom_renderer = std::make_shared<BloomRenderer>(
        bloom_shader, blur_shader,
        m_view->color_texture(0), m_view->color_texture(1));

    auto *arena = make_arena();
    if (!arena)
        return false;

    auto *bumber_car_template = make_bumber_car();
    if (!bumber_car_template)
        return false;

    constexpr vec3 colors[] = 
    { 
        vec3(0.2, 1.0, 0.2), vec3(0.8, 0.5, 1.0),
        vec3(1.0, 1.0, 0.2), vec3(0.2, 1.0, 1.0),
    };

    for (int i = 0; i < 4; i++)
        make_ai(*bumber_car_template, vec3((i - 2) * 5, 0, 10), colors[i]);

    auto *player = bumber_car_template;
    player->add_component<PlayerController>();
    set_bumper_car_color(*player, vec3(1, 0.2, 0.2));

    auto *camera = make_cameras(*player);
    if (!camera)
        return false;

    // make_sky_box(skybox_texture);

    m_renderer->set_camera(*camera);
    m_renderer->on_world_updated(*m_world);

    m_sky_box_renderer->set_camera(*camera);
    m_sky_box_renderer->on_world_updated(*m_world);

    m_world->init();
    ThreadPool::finished_loading();
    return true;
}

void BumberCarsScene::on_render(float delta)
{
    m_world->step_physics(delta);
    m_world->update(delta);
    m_collision_resolver->resolve(*m_world);

    m_view->render();
    m_bloom_renderer->pre_render();
    m_bloom_renderer->render();

    if (Input::is_key_down('1'))
        m_renderer->set_camera(*m_in_car_camera);
    if (Input::is_key_down('2'))
        m_renderer->set_camera(*m_free_camera);
}

void BumberCarsScene::on_resize(int width, int height)
{
    m_view->resize(width, height);
    m_bloom_renderer->resize_viewport(width, height);
}

