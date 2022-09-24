/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include "engine/graphics/renderer/renderer.hpp"
#include "texture.hpp"
#include <utility>
#include <vector>

namespace Engine
{

    class RenderTexture : public Texture
    {
    public:
		static std::shared_ptr<RenderTexture> construct(
            std::vector<std::shared_ptr<Renderer>> pipeline);

        ~RenderTexture() override;

        void add_color_texture();

        void resize(int width, int height);
        void render();

        inline int width() const { return m_viewport_width; }
        inline int height() const { return m_viewport_height; }
        inline int texture_width() const { return m_texture_width; }
        inline int texture_height() const { return m_texture_height; }

        std::shared_ptr<Texture> color_texture(int index);

    private:
        RenderTexture(std::vector<std::shared_ptr<Renderer>> pipeline, GLuint fbo, GLuint color_texture, GLuint depth_texture)
            : Texture(color_texture)
            , m_pipeline(std::move(pipeline))
            , m_fbo(fbo)
            , m_color_textures({ color_texture })
            , m_depth_texture(depth_texture)
        {
            m_has_loaded = true;
        }

        std::vector<std::shared_ptr<Renderer>> m_pipeline;
        int m_viewport_width { 0 };
        int m_viewport_height { 0 };
        int m_texture_width { 0 };
        int m_texture_height { 0 };

        GLuint m_fbo;
        std::vector<GLuint> m_color_textures;
        GLuint m_depth_texture;

    };

}

