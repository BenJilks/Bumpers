/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "bloom_renderer.hpp"

#include "blur_renderer.hpp"
#include "engine/graphics/shader.hpp"
#include "engine/graphics/texture/render_texture.hpp"
#include "engine/graphics/texture/texture.hpp"
#include <utility>
using namespace Engine;

constexpr int blur_scale = 3;

BloomRenderer::BloomRenderer(std::shared_ptr<Shader> shader,
    std::shared_ptr<Shader> const& blur_shader,
    std::shared_ptr<Texture> standard_view,
    std::shared_ptr<Texture> const& light_view)
    : PostProcessRenderer(std::move(shader))
    , m_standard_view(std::move(standard_view))
{
    m_blur_horizontal_renderer = std::make_shared<BlurRenderer>(blur_shader, light_view, glm::vec2(1, 0));
    m_blur_horizontal_view = RenderTexture::construct({ m_blur_horizontal_renderer });

    m_blur_vertical_renderer = std::make_shared<BlurRenderer>(blur_shader, m_blur_horizontal_view, glm::vec2(0, 1));
    m_blur_vertical_view = RenderTexture::construct({ m_blur_vertical_renderer });
}

BloomRenderer::~BloomRenderer() = default;

void BloomRenderer::pre_render()
{
    m_blur_horizontal_view->render();
    m_blur_vertical_view->render();
}

static int next_power_of_two(int value)
{
    int power = 1;
    while (power < value) {
        power *= 2;
    }

    return power;
}

void BloomRenderer::bind_inputs()
{
    m_shader->load_int("standard_view", 0);
    m_shader->load_int("light_only_view", 1);

#ifdef WEBASSEMBLY
    auto standard_texture_width = next_power_of_two(width());
    auto standard_texture_height = next_power_of_two(height());
    m_shader->load_vec2("standard_view_scale", glm::vec2((float)width() / (float)standard_texture_width, (float)height() / (float)standard_texture_height));

    m_shader->load_vec2("light_only_view_scale", glm::vec2((float)m_blur_vertical_view->width() / (float)m_blur_vertical_view->texture_width(), (float)m_blur_horizontal_view->height() / (float)m_blur_horizontal_view->texture_height()));
#endif

    m_standard_view->bind(0);
    m_blur_vertical_view->bind(1);
}

void BloomRenderer::on_resize(int width, int height)
{
    m_blur_horizontal_renderer->resize_view(width, height);
    m_blur_horizontal_view->resize(width / blur_scale, height / blur_scale);

    m_blur_vertical_renderer->resize_view(width / blur_scale, height / blur_scale);
    m_blur_vertical_view->resize(width / blur_scale, height / blur_scale);
}
