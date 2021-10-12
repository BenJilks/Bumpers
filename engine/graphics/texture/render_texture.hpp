#pragma once

#include "texture.hpp"

namespace Engine
{

    class RenderTexture : public Texture
    {
    public:
		static std::shared_ptr<RenderTexture> construct(int width, int height);
        static void unbind_for_rendering();
        ~RenderTexture();

        void resize(int width, int height);
        void bind_for_rendering();

    private:
        RenderTexture(GLuint fbo, GLuint texture)
            : Texture(texture)
            , m_fbo(fbo)
        {
        }

        GLuint m_fbo;

    };

}
