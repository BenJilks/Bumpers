/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "post_process_renderer.hpp"

#include <utility>
#include "engine/graphics/mesh/mesh.hpp"
#include "engine/graphics/mesh/mesh_builder.hpp"
#include "engine/graphics/texture/texture.hpp"
#include "engine/graphics/shader.hpp"
using namespace Engine;
using namespace glm;

PostProcessRenderer::PostProcessRenderer(std::shared_ptr<Shader> shader)
    : Renderer(std::move(shader))
{
    m_full_screen_quad = MeshBuilder()
        .add_quad(vec2(1), true)
        .build();
}

PostProcessRenderer::~PostProcessRenderer() = default;

glm::mat4 PostProcessRenderer::projection_matrix(int width, int height)
{
    on_resize(width, height);
    return mat4(1);
}

void PostProcessRenderer::on_render()
{
    bind_inputs();
    m_full_screen_quad->draw();
}
