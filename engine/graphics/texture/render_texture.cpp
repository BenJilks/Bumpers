/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "render_texture.hpp"
#include <GL/glew.h>
#include <cassert>
#include <utility>
using namespace Engine;

std::shared_ptr<RenderTexture> RenderTexture::construct(
    std::vector<std::shared_ptr<Renderer>> pipeline)
{
    GLuint color_texture;
    glGenTextures(1, &color_texture);
    glBindTexture(GL_TEXTURE_2D, color_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLuint depth_texture;
    glGenTextures(1, &depth_texture);
    glBindTexture(GL_TEXTURE_2D, depth_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return std::shared_ptr<RenderTexture>(new RenderTexture(std::move(pipeline), fbo, color_texture, depth_texture));
}

void RenderTexture::add_color_texture()
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texture_width, m_texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    m_color_textures.push_back(texture);

    auto index = m_color_textures.size() - 1;
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, texture, 0);

    std::vector<GLuint> attachments(m_color_textures.size());
    for (int i = 0; i < m_color_textures.size(); i++) {
        attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    glDrawBuffers(attachments.size(), attachments.data());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTexture::~RenderTexture()
{
    glDeleteBuffers(1, &m_fbo);
    glDeleteTextures(1, &m_depth_texture);
}

std::shared_ptr<Texture> RenderTexture::color_texture(int index)
{
    assert(index < m_color_textures.size());
    return std::make_shared<Texture>(m_color_textures[index]);
}

void RenderTexture::render()
{
    for (auto& renderer : m_pipeline) {
        renderer->pre_render();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto& renderer : m_pipeline) {
        renderer->render();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static int next_power_of_two(int value)
{
    int power = 1;
    while (power < value) {
        power *= 2;
    }

    return power;
}

void RenderTexture::resize(int width, int height)
{
    m_viewport_width = width;
    m_viewport_height = height;

#ifdef WEBASSEMBLY
    m_texture_width = next_power_of_two(width);
    m_texture_height = next_power_of_two(height);
#else
    m_texture_width = width;
    m_texture_height = height;
#endif

    for (auto& renderer : m_pipeline) {
        renderer->resize_viewport(m_viewport_width, m_viewport_height);
    }

    for (auto texture : m_color_textures) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_texture_width, m_texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    glBindTexture(GL_TEXTURE_2D, m_depth_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_texture_width, m_texture_height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr);
    glBindTexture(GL_TEXTURE_2D, 0);
}
