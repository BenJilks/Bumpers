/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "blur_renderer.hpp"

#include "engine/graphics/shader.hpp"
#include "engine/graphics/texture/texture.hpp"
#include <utility>
using namespace Engine;

constexpr int blur_amount = 8;

BlurRenderer::BlurRenderer(std::shared_ptr<Shader> shader, std::shared_ptr<Texture> view, glm::vec2 direction)
    : PostProcessRenderer(std::move(shader))
    , m_view(std::move(view))
    , m_direction(direction)
{
}

BlurRenderer::~BlurRenderer() = default;

static int next_power_of_two(int value)
{
    int power = 1;
    while (power < value) {
        power *= 2;
    }

    return power;
}

void BlurRenderer::bind_inputs()
{
    m_shader->load_vec2("screen_size", glm::vec2(width(), height()));
    m_shader->load_vec2("direction", m_direction);
    m_shader->load_int("texture", 0);

#ifdef WEBASSEMBLY
    m_shader->load_vec2("view_scale", m_view_scale);
#endif
    m_view->bind(0);
}

void BlurRenderer::resize_view(int width, int height)
{
#ifdef WEBASSEMBLY
    auto texture_width = next_power_of_two(width);
    auto texture_height = next_power_of_two(height);
    m_view_scale = glm::vec2((float)width / (float)texture_width, (float)height / (float)texture_height);
#endif
}

void BlurRenderer::on_resize(int, int)
{
}
