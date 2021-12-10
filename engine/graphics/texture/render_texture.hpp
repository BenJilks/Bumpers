#pragma once

#include "engine/graphics/renderer/renderer.hpp"
#include "texture.hpp"
#include <vector>

namespace Engine
{

    class RenderTexture : public Texture
    {
    public:
		static std::shared_ptr<RenderTexture> construct(
            std::vector<std::shared_ptr<Renderer>> pipeline);

        ~RenderTexture();

        void add_color_texture();

        void resize(int width, int height);
        void render();

        std::shared_ptr<Texture> color_texture(int index);

    private:
        RenderTexture(std::vector<std::shared_ptr<Renderer>> pipeline, GLuint fbo, GLuint color_texture, GLuint depth_texture)
            : Texture(color_texture)
            , m_pipeline(pipeline)
            , m_fbo(fbo)
            , m_color_textures({ color_texture })
            , m_depth_texture(depth_texture)
        {
            m_has_loaded = true;
        }

        std::vector<std::shared_ptr<Renderer>> m_pipeline;
        int m_width;
        int m_height;

        GLuint m_fbo;
        std::vector<GLuint> m_color_textures;
        GLuint m_depth_texture;

    };

}

