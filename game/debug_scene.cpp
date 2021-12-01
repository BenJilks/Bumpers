#include "debug_scene.hpp"
#include "debug_camera_movement.hpp"
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
#include "engine/input.hpp"
#include "gameobject/world.hpp"
#include "gameobject/camera.hpp"
#include "gameobject/transform.hpp"
#include "gameobject/mesh_render.hpp"
#include "gameobject/light.hpp"
#include <glm/gtx/string_cast.hpp>
#include <iostream>
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

void DebugScene::make_test_object()
{
    auto *test_object = ColladaLoader::from_file(*m_world, ASSETS + "/models/bumper.dae", 
        [&](Object::GameObject &object, Engine::MeshBuilder &builder, ColladaLoader::ModelMetaData meta_data)
    {
        object.add_component<MeshRender>(builder.build(), m_renderer, meta_data.material);

        auto &transform = object.add_component<Transform>();
        transform.set_position(meta_data.translation);
        transform.set_scale(meta_data.scale);
        transform.set_rotation(meta_data.rotation);
    });

    {
        auto &test_light = m_world->add_child();
        auto &test_light_transform = test_light.add_component<Transform>();
        test_light.add_component<Light>(vec3(1, 0, 0));
        test_light.add_component<Circle>(0);

        test_light_transform.translate(vec3(0, -6, 0));
    }

    {
        auto &test_light = m_world->add_child();
        auto &test_light_transform = test_light.add_component<Transform>();
        test_light.add_component<Light>(vec3(0, 1, 0));
        test_light.add_component<Circle>(glm::radians(180.0f));

        test_light_transform.translate(vec3(0, -6, 0));
    }
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

    make_test_object();
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

    m_view->render();
    m_bloom_renderer->pre_render();
    m_bloom_renderer->render();
}

void DebugScene::on_resize(int width, int height)
{
    m_view->resize(width, height);
    m_bloom_renderer->resize_viewport(width, height);
}

