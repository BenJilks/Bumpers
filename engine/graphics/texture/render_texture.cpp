#include "render_texture.hpp"
#include <GL/glew.h>
using namespace Engine;

std::shared_ptr<RenderTexture> RenderTexture::construct(int width, int height)
{
    GLuint fbo;
    GLuint texture;

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, texture);

    return std::shared_ptr<RenderTexture>(new RenderTexture(fbo, texture));
}

RenderTexture::~RenderTexture()
{
    glDeleteBuffers(1, &m_fbo);
}

void RenderTexture::unbind_for_rendering()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTexture::bind_for_rendering()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void RenderTexture::resize(int width, int height)
{
    bind(0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    unbind(0);
}
