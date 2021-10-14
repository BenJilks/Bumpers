// TODO: Re-implement this

/*
#include "gui_renderer.hpp"
#include "engine/graphics/mesh/mesh.hpp"
#include "engine/graphics/texture/texture.hpp"
#include "engine/graphics/shader.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include "gameobject/mesh_render.hpp"
#include "gameobject/gui/control.hpp"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
using namespace Engine;
using namespace Object;
using namespace glm;

void GuiRenderer::start_frame()
{
}

void GuiRenderer::render(const Object::GameObject &gameobject)
{
    auto *control = gameobject.first<Gui::Control>();
    if (!control)
        return;
    
    auto *mesh_render = gameobject.first<MeshRender>();
    if (!mesh_render || &mesh_render->renderer() != this)
        return;

    auto *transform = gameobject.first<Transform>();
    if (!transform)
        return;

    const auto anchor = control->anchor();
    const auto origin = glm::translate(mat4(1.0f), vec3(anchor.x, anchor.y, 0));
    const auto to_pixel_space = vec2(1.0f / m_width, 1.0f / m_height) * 2.0f;
    const auto global_transform = glm::scale(mat4(1.0f), vec3(to_pixel_space, 1.0f)) 
        * transform->global_transform(gameobject);

    const auto &material = mesh_render->material();
	m_shader->bind();
	m_shader->load_int("diffuse_map", 0);
    m_shader->load_matrix("mvp", m_projection_matrix * m_view * origin * global_transform);
	m_shader->load_vec3("color", material.color);

    material.diffuse_map->bind(0);
    mesh_render->mesh().draw();
}

void GuiRenderer::resize_viewport(int width, int height)
{
	m_projection_matrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
	m_width = width;
	m_height = height;
}
*/
