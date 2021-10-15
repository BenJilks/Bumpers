#pragma once

#include "texture.hpp"
#include <string>

namespace Engine
{

    class CubeMapTexture : public Texture
    {
    public:
		static std::shared_ptr<CubeMapTexture> construct(const std::string &file_path);

    private:
        CubeMapTexture(GLuint texture)
            : Texture(texture)
        {
        }

    };

}
