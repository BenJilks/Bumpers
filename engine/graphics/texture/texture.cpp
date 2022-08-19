/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "texture.hpp"
#include <GL/glew.h>
#include <iostream>
#include <cstring>
using namespace Engine;

#ifdef WIN32
#include <Windows.h>
#endif

void Texture::unbind(int slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(GLuint texture)
	: m_texture(texture)
{
#ifdef WIN32
	m_loader_thread_mutex = CreateMutex(NULL, FALSE, NULL);
#endif
}

bool Texture::has_loaded() const
{
	if (m_has_loaded)
		return true;

#ifdef WIN32
	WaitForSingleObject(m_loader_thread_mutex, INFINITE);
	auto has_loaded = m_has_data_loaded;
	ReleaseMutex(m_loader_thread_mutex);
	return has_loaded;
#else
	std::lock_guard<std::mutex> guard(m_loader_thread_mutex);
	return m_has_data_loaded;
#endif
}

void Texture::bind(int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

Texture::~Texture()
{
	glDeleteTextures(1, &m_texture);

#ifdef WIN32
	CloseHandle(m_loader_thread_mutex);
#endif
}

