#include "debug_scene.hpp"
#include "config.hpp"
#include "gameobject/camera.hpp"
#include "gameobject/transform.hpp"
#include "gameobject/debug_camera_movement.hpp"
#include "gameobject/mesh_render.hpp"
#include "gameobject/spin.hpp"
using namespace Game;

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

Object::GameObject *DebugScene::make_test_object()
{
    auto mesh = MeshBuilder()
        .add_quad(vec2(1), false)
        .build();
    if (!mesh)
        return nullptr;

    auto &test_object = m_world->add_child();
    test_object.add_component<Transform>();
    test_object.add_component<MeshRender>(mesh, nullptr, m_renderer);
    test_object.add_component<Spin>();
    return test_object;
}

bool DebugScene::init()
{
    auto shader = Shader::construct(ASSETS + "/shaders/default.glsl");

    m_world = std::make_unique<World>();
    m_renderer = std::make_shared<Renderer>(shader, 800, 600);

    auto *camera = make_camera();
    if (!camera)
        return false;

    auto *test_object = make_test_object();
    if (!test_object)
        return false;

    m_renderer->set_camera(*camera);
    m_world->init();
    return true;
}

void DebugScene::on_render(float delta)
{
    m_world->step_physics(delta);
    m_world->update(delta);

    m_renderer->start_frame();
    m_world->for_each([this](const GameObject &object)
    {
        m_renderer->render(object);
        return ItoratorDecision::Continue;
    });
}

void DebugScene::on_resize(int width, int height)
{
    m_renderer->resize_viewport(width, height);
}
