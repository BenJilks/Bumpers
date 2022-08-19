/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "blur_renderer.hpp"
#include "engine/graphics/texture/texture.hpp"
#include "engine/graphics/texture/render_texture.hpp"
#include "engine/graphics/shader.hpp"
using namespace Engine;
using namespace glm;

constexpr int blur_amount = 8;

BlurRenderer::BlurRenderer(std::shared_ptr<Shader> shader, 
    std::shared_ptr<Texture> view, glm::vec2 direction)
    : PostProcessRenderer(shader)
    , m_view(view)
    , m_direction(direction)
{
}

BlurRenderer::~BlurRenderer()
{
}

void BlurRenderer::bind_inputs()
{
    m_shader->load_vec2("screen_size", vec2(width(), height()));
    m_shader->load_vec2("direction", m_direction);
    m_shader->load_int("texture", 0);
    m_view->bind(0);
}

void BlurRenderer::on_resize(int, int)
{
}
