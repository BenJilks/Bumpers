#include "image_texture.hpp"
#include "engine/assets/thread_pool.hpp"
#include <GL/glew.h>
#include <iostream>
#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
using namespace Engine;

#ifdef WIN32
#include <Windows.h>
#endif

std::shared_ptr<ImageTexture> ImageTexture::construct(const std::string &file_path)
{
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    auto texture = std::shared_ptr<ImageTexture>(new ImageTexture(texture_id));
    auto texture_weak = std::weak_ptr<ImageTexture>(texture);
    texture->m_name = file_path;

    // FIXME: We leak a little bit of memory here, as Windows doesn't like freeing 
    //        memory from a non-main thread.
    char *file_path_copy = new char[file_path.size() + 1];
    strcpy(file_path_copy, file_path.c_str());

    ThreadPool::queue_task([file_path_copy, texture_weak]()
    {
        int width, height, components;
        uint8_t* data = stbi_load(file_path_copy, &width, &height, &components, 4);
        if (!data)
        {
            std::cerr << "Error loading texture '" << file_path_copy << "': " << strerror(errno) << "\n";
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

