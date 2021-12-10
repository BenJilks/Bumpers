#include "image_texture.hpp"
#include "engine/assets/thread_pool.hpp"
#include <GL/glew.h>
#include <iostream>
#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
using namespace Engine;

std::shared_ptr<ImageTexture> ImageTexture::construct(const std::string &file_path)
{
    GLuint texture_id;
    glGenTextures(1, &texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    auto texture = std::shared_ptr<ImageTexture>(new ImageTexture(texture_id));
    auto texture_weak = std::weak_ptr<ImageTexture>(texture);
    ThreadPool::queue_task([file_path, texture_weak]()
    {
        int width, height, components;
        uint8_t* data = stbi_load(file_path.c_str(), &width, &height, &components, 4);
        if (!data)
        {
            std::cerr << "Error loading texture '" << file_path << "': " << strerror(errno) << "\n";
            return;
        }

        auto texture = texture_weak.lock();
        if (!texture)
            return;

        texture->m_data = data;
        texture->m_width = width;
        texture->m_height = height;
        texture->m_has_loaded = true;
    });

	return texture;
}

void ImageTexture::bind(int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_texture);

    if (m_data != nullptr)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(m_data);
        m_data = nullptr;
    }
}

