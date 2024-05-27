/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "renderer.hpp"
#include "engine/graphics/shader.hpp"
#include "gameobject/gameobject.hpp"
#include "gameobject/transform.hpp"
#include <GL/glew.h>
using namespace Engine;
using namespace Object;

void Renderer::resize_viewport(int width, int height)
{
    m_projection_matrix = projection_matrix(width, height);
    m_width = width;
    m_height = height;
}

void Renderer::view_matrix(GameObject const& camera)
{
    auto const* transform = camera.first<Transform>();
    if (!transform) {
        return;
    }

    m_view = transform->global_inverse_transform(camera);
    m_camera_position = transform->position();
}

void Renderer::render()
{
    glViewport(0, 0, m_width, m_height);

    if (m_camera) {
        view_matrix(*m_camera);
    } else {
        m_view = glm::mat4(1);
    }

    m_shader->bind();
    on_start_frame();
    on_render();
}
