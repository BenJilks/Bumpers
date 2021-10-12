#pragma once

#include "texture.hpp"
#include <string>

namespace Engine
{

    class ImageTexture : public Texture
    {
    public:
		static std::shared_ptr<ImageTexture> construct(const std::string &file_path);

    private:
        ImageTexture(GLuint texture)
            : Texture(texture)
        {
        }

    };

}
