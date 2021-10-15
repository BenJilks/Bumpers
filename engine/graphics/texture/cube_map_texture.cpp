#include "cube_map_texture.hpp"
#include <array>
#include <cstring>
#include <GL/gl.h>
#include <stb_image.h>
#include <iostream>
using namespace Engine;

constexpr std::array s_face_names = 
{ 
    "positive_x", "negative_x",
    "positive_y", "negative_y",
    "positive_z", "negative_z",
};

std::shared_ptr<CubeMapTexture> CubeMapTexture::construct(const std::string &file_path_prefix)
{
    GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    for (int i = 0; i < s_face_names.size(); i++)
    {
        auto file_path = file_path_prefix + "_" + s_face_names[i] + ".jpg";

        int width, height, channels;
        auto data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);
	    if (!data)
	    {
	        std::cerr << "Error loading texture '" << file_path << "': " << std::strerror(errno) << "\n";
	        return nullptr;
	    }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
            0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  
    return std::shared_ptr<CubeMapTexture>(new CubeMapTexture(texture));
}

void CubeMapTexture::bind(int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
}
