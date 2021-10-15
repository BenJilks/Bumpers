#pragma once

#include "engine/graphics/renderer/renderer.hpp"
#include "texture.hpp"
#include <vector>

namespace Engine
{

    class RenderTexture : public Texture
    {
    public:
		static std::shared_ptr<RenderTexture> construct(std::shared_ptr<Renderer> renderer);
        ~RenderTexture();

        void add_color_texture();

        void resize(int width, int height);
        void render();

        std::shared_ptr<Texture> color_texture(int index);

    private:
        RenderTexture(std::shared_ptr<Renderer> renderer, GLuint fbo, GLuint color_texture, GLuint depth_texture)
            : Texture(color_texture)
            , m_renderer(renderer)
            , m_fbo(fbo)
            , m_color_textures({ color_texture })
            , m_depth_texture(depth_texture)
        {
        }

        std::shared_ptr<Renderer> m_renderer;
        int m_width;
        int m_height;

        GLuint m_fbo;
        std::vector<GLuint> m_color_textures;
        GLuint m_depth_texture;

    };

}
