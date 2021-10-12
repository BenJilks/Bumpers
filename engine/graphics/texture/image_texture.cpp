#include "image_texture.hpp"
#include <GL/glew.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
using namespace Engine;

std::shared_ptr<ImageTexture> ImageTexture::construct(const std::string &file_path)
{
	int width, height, components;
	uint8_t* data = stbi_load(file_path.c_str(), &width, &height, &components, 4);
	if (!data)
	{
		std::cerr << "Error loading texture '" << file_path << "': " << strerror(errno) << "\n";
		return nullptr;
	}

    GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
	return std::shared_ptr<ImageTexture>(new ImageTexture(texture));
}
