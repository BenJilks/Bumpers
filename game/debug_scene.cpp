#include "debug_scene.hpp"
#include "debug_camera_movement.hpp"
#include "spin.hpp"
#include "config.hpp"
#include "engine/assets/obj_loader.hpp"
#include "engine/graphics/mesh/mesh_builder.hpp"
#include "engine/graphics/texture/image_texture.hpp"
#include "engine/graphics/shader.hpp"
#include "engine/graphics/renderer/standard_renderer.hpp"
#include "gameobject/world.hpp"
#include "gameobject/camera.hpp"
#include "gameobject/transform.hpp"
#include "gameobject/mesh_render.hpp"
#include "gameobject/light.hpp"
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
    virtual ~Circle() final
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
    auto *test_object = ObjLoader::from_file(*m_world, ASSETS + "/models/bumper.obj", 
        [&](Object::GameObject &object, Engine::MeshBuilder &builder, ObjLoader::ModelMetaData meta_data)
    {
        object.add_component<Transform>();
        object.add_component<MeshRender>(builder.build(), m_renderer, meta_data.material);
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

bool DebugScene::init()
{
    auto shader = Shader::construct(ASSETS + "/shaders/default.glsl");

    m_world = std::make_unique<World>();
    m_renderer = std::make_shared<StandardRenderer>(shader);

    auto *camera = make_camera();
    if (!camera)
        return false;

    make_test_object();

    m_renderer->set_camera(*camera);
    m_world->init();
    m_renderer->on_world_updated(*m_world);
    return true;
}

void DebugScene::on_render(float delta)
{
    m_world->step_physics(delta);
    m_world->update(delta);
    m_renderer->render();
}

void DebugScene::on_resize(int width, int height)
{
    m_renderer->resize_viewport(width, height);
}
