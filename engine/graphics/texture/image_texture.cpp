/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "image_texture.hpp"
#include "engine/assets/thread_pool.hpp"
#include "engine/assets/asset_repository.hpp"
#include <GL/glew.h>
#include <iostream>
#include <memory>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <map>
using namespace Engine;

#ifdef WIN32
#include <Windows.h>
#endif

std::map<std::string, std::weak_ptr<ImageTexture>> s_loaded_textures;

std::shared_ptr<ImageTexture> ImageTexture::construct(const AssetRepository &assets, std::string_view name)
{
    auto loaded_texture_index = s_loaded_textures.find(std::string(name));
    if (loaded_texture_index != s_loaded_textures.end())
    {
        auto loaded_texture = loaded_texture_index->second;
        if (!loaded_texture.expired())
            return loaded_texture.lock();

        s_loaded_textures.erase(loaded_texture_index);
    }

    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    auto texture = std::shared_ptr<ImageTexture>(new ImageTexture(texture_id));
    auto texture_weak = std::weak_ptr<ImageTexture>(texture);
    texture->m_name = name;
    s_loaded_textures.insert(std::make_pair(std::string(name), texture));

    ThreadPool::queue_task([assets = assets.copy(), name = std::string(name), texture_weak]()
    {
        auto stream = std::move(assets->open(name));
        stream->seekg(0, std::ios::end);
        auto size = static_cast<long>(stream->tellg());
        stream->seekg(0, std::ios::beg);

        std::vector<char> buffer(size);
        if (!stream->read(buffer.data(), size))
        {
            std::cerr << "Error: Unable to read texture " << name << "\n";
            return;
        }

        int width, height, channels;
        auto stbi_buffer = reinterpret_cast<const stbi_uc *>(buffer.data());
        auto stbi_size = static_cast<int>(size);
        uint8_t *data = stbi_load_from_memory(stbi_buffer, stbi_size, &width, &height, &channels, 4);
        if (!data)
        {
            std::cerr << "Error loading texture '" << name << "': " << strerror(errno) << "\n";
            return;
        }

        auto texture = texture_weak.lock();
        if (!texture)
            return;

        texture->m_data = data;
        texture->m_width = width;
        texture->m_height = height;

#ifdef WIN32
        WaitForSingleObject(texture->m_loader_thread_mutex, INFINITE);
        texture->m_has_data_loaded = true;
        ReleaseMutex(texture->m_loader_thread_mutex);
#else
        std::lock_guard<std::mutex> guard(texture->m_loader_thread_mutex);
        texture->m_has_data_loaded = true;
#endif
    });

	return texture;
}

void ImageTexture::bind(int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_texture);

    if (m_has_loaded)
        return;

#ifdef WIN32
    WaitForSingleObject(m_loader_thread_mutex, INFINITE);
#else
    std::lock_guard<std::mutex> guard(m_loader_thread_mutex);
#endif
    if (m_has_data_loaded)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(m_data);
        m_data = nullptr;
        m_has_loaded = true;
    }

#ifdef WIN32
    ReleaseMutex(m_loader_thread_mutex);
#endif
}

