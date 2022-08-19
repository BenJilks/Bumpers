/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "bloom_renderer.hpp"
#include "blur_renderer.hpp"
#include "engine/graphics/texture/texture.hpp"
#include "engine/graphics/texture/render_texture.hpp"
#include "engine/graphics/shader.hpp"
using namespace Engine;
using namespace glm;

BloomRenderer::BloomRenderer(std::shared_ptr<Shader> shader, 
                             std::shared_ptr<Shader> blur_shader,
                             std::shared_ptr<Texture> standard_view,
                             std::shared_ptr<Texture> light_view)
    : PostProcessRenderer(shader)
    , m_standard_view(standard_view)
{
    auto blur_horizontal_renderer = std::make_shared<BlurRenderer>(blur_shader, light_view, vec2(1, 0));
    m_blur_horizontal_view = RenderTexture::construct({ blur_horizontal_renderer });

    auto blur_virtual_renderer = std::make_shared<BlurRenderer>(blur_shader, m_blur_horizontal_view, vec2(0, 1));
    m_blur_virtual_view = RenderTexture::construct({ blur_virtual_renderer });
}

BloomRenderer::~BloomRenderer()
{

}

void BloomRenderer::pre_render()
{
    m_blur_horizontal_view->render();
    m_blur_virtual_view->render();
}

void BloomRenderer::bind_inputs()
{
    m_shader->load_int("standard_view", 0);
    m_shader->load_int("light_only_view", 1);
    m_standard_view->bind(0);
    m_blur_virtual_view->bind(1);
}

void BloomRenderer::on_resize(int width, int height)
{
    constexpr int scale = 3;
    m_blur_horizontal_view->resize(width / scale, height / scale);
    m_blur_virtual_view->resize(width / scale, height / scale);
}
