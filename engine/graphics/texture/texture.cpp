#include "texture.hpp"
#include <GL/glew.h>
#include <iostream>
#include <cstring>
using namespace Engine;

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_texture);
}

void Texture::bind(int slot) const
{
	glBindTexture(GL_TEXTURE_2D, m_texture);
}
