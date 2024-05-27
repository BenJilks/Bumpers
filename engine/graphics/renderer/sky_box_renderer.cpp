/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "sky_box_renderer.hpp"
#include "engine/graphics/mesh/mesh.hpp"
#include "engine/graphics/shader.hpp"
#include "engine/graphics/texture/texture.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/mesh_render.hpp"
#include "gameobject/transform.hpp"
#include <GL/glew.h>
#include <glm/gtx/transform.hpp>
using namespace Engine;
using namespace Object;
using namespace glm;

#define MAX_LIGHT_COUNT 10

SkyBoxRenderer::SkyBoxRenderer(std::shared_ptr<Shader> shader)
    : Renderer(std::move(shader))
{
}

SkyBoxRenderer::~SkyBoxRenderer()
{
}

void SkyBoxRenderer::on_world_updated(Object::GameObject& world)
{
    m_mesh_renders.clear();
    world.for_each([&](GameObject const& game_object) {
        const auto* transform = game_object.first<Transform>();
        if (!transform) {
            return IteratorDecision::Continue;
        }

        const auto* mesh_render = game_object.first<MeshRender>();
        if (!mesh_render || &mesh_render->renderer() != this) {
            return IteratorDecision::Continue;
        }

        m_mesh_renders.push_back(MeshRenderData {
            .game_object = game_object,
            .transform = *transform,
            .mesh_render = *mesh_render,
        });

        return IteratorDecision::Continue;
    });
}

glm::mat4 SkyBoxRenderer::projection_matrix(int width, int height)
{
    float aspect_ratio = (float)width / (float)height;
    return glm::perspective(70.0f, aspect_ratio, 0.1f, 100.0f);
}

void SkyBoxRenderer::on_start_frame()
{
    m_view[3][0] = 0;
    m_view[3][1] = 0;
    m_view[3][2] = 0;
}

void SkyBoxRenderer::on_render()
{
    glDisable(GL_DEPTH_TEST);

    for (auto const& data : m_mesh_renders) {
        auto global_transform = data.transform.global_transform(data.game_object);
        auto const& material = data.mesh_render.material();
        m_shader->load_int("diffuse_map", 0);
        m_shader->load_matrix("mvp", m_projection_matrix * m_view * global_transform);

        if (material.diffuse_map && material.diffuse_map->has_loaded()) {
            material.diffuse_map->bind(0);
        }

        data.mesh_render.mesh().draw();
        Texture::unbind(0);
    }

    glEnable(GL_DEPTH_TEST);
}
