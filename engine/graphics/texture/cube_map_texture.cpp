/*
 * Copyright (c) 2022, Ben Jilks <benjyjilks@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "cube_map_texture.hpp"
#include "engine/assets/asset_repository.hpp"
#include "engine/assets/thread_pool.hpp"
#include <GL/glew.h>
#include <array>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <memory>
#include <stb_image.h>
#include <tuple>
using namespace Engine;

#ifdef WIN32
#include <Windows.h>
#endif

constexpr std::array s_face_names = {
    "positive_x",
    "negative_x",
    "positive_y",
    "negative_y",
    "positive_z",
    "negative_z",
};

std::shared_ptr<CubeMapTexture> CubeMapTexture::construct(AssetRepository const& assets, std::string_view name_prefix)
{
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    auto texture = std::shared_ptr<CubeMapTexture>(new CubeMapTexture(texture_id));
    auto texture_weak = std::weak_ptr<CubeMapTexture>(texture);

    ThreadPool::queue_task([name_prefix = std::string(name_prefix), assets = assets.copy(), texture_weak]() {
        std::vector<std::tuple<uint8_t*, int, int>> faces;
        for (int i = 0; i < s_face_names.size(); i++) {
            auto name = std::string(name_prefix) + "_" + std::string(s_face_names[i]) + ".jpg";
            auto stream = std::move(assets->open(name));
            stream->seekg(0, std::ios::end);
            auto size = static_cast<long>(stream->tellg());
            stream->seekg(0, std::ios::beg);

            std::vector<char> buffer(size);
            if (!stream->read(buffer.data(), size)) {
                std::cerr << "Error: Unable to read texture " << name << "\n";
                return;
            }

            int width;
            int height;
            int channels;

            auto const* stbi_buffer = reinterpret_cast<const stbi_uc*>(buffer.data());
            auto stbi_size = static_cast<int>(size);
            uint8_t* data = stbi_load_from_memory(stbi_buffer, stbi_size, &width, &height, &channels, 0);
            if (!data) {
                std::cerr << "Error loading texture '" << name << "': " << std::strerror(errno) << "\n";
                return;
            }

            faces.emplace_back(data, width, height);
        }

        auto texture = texture_weak.lock();
        if (!texture) {
            return;
        }

        texture->m_data = faces;

#ifdef WIN32
        WaitForSingleObject(texture->m_loader_thread_mutex, INFINITE);
        texture->m_has_data_loaded = true;
        ReleaseMutex(texture->m_loader_thread_mutex);
#else
        std::lock_guard<std::mutex> const guard(texture->m_loader_thread_mutex);
        texture->m_has_data_loaded = true;
#endif
    });

    return texture;
}

void CubeMapTexture::bind(int slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

    if (m_has_loaded) {
        return;
    }

#ifdef WIN32
    WaitForSingleObject(m_loader_thread_mutex, INFINITE);
#else
    std::lock_guard<std::mutex> guard(m_loader_thread_mutex);
#endif

    if (m_has_data_loaded) {
        for (int i = 0; i < m_data.size(); i++) {
            auto const& [data, width, height] = m_data[i];
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        m_data.clear();
        m_has_loaded = true;
    }

#ifdef WIN32
    ReleaseMutex(m_loader_thread_mutex);
#endif
}
