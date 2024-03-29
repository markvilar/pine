#include "pine/platform/opengl/framebuffer.hpp"

#include <glad/glad.h>

#include "pine/pch.hpp"

namespace pine
{

static constexpr uint32_t s_max_framebuffer_size = 8192;

OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecs& specs)
    : m_specification(specs)
{
    invalidate();
}

OpenGLFramebuffer::~OpenGLFramebuffer()
{
    glDeleteFramebuffers(1, &m_renderer_id);
    glDeleteTextures(1, &m_color_attachment);
    glDeleteTextures(1, &m_depth_attachment);
}

void OpenGLFramebuffer::invalidate()
{
    if (m_renderer_id)
    {
        glDeleteFramebuffers(1, &m_renderer_id);
        glDeleteTextures(1, &m_color_attachment);
        glDeleteTextures(1, &m_depth_attachment);
    }
    glCreateFramebuffers(1, &m_renderer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_renderer_id);

    // Color attachment.
    glCreateTextures(GL_TEXTURE_2D, 1, &m_color_attachment);
    glBindTexture(GL_TEXTURE_2D, m_color_attachment);
    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        static_cast<GLsizei>(m_specification.width),
        static_cast<GLsizei>(m_specification.height),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        m_color_attachment,
        0);

    // Depth attachment.
    glCreateTextures(GL_TEXTURE_2D, 1, &m_depth_attachment);
    glBindTexture(GL_TEXTURE_2D, m_depth_attachment);
    glTexStorage2D(GL_TEXTURE_2D,
        1,
        GL_DEPTH24_STENCIL8,
        static_cast<GLsizei>(m_specification.width),
        static_cast<GLsizei>(m_specification.height));
    glFramebufferTexture2D(GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_TEXTURE_2D,
        m_depth_attachment,
        0);

    PINE_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER)
            == GL_FRAMEBUFFER_COMPLETE,
        "Framebuffer is incomplete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLFramebuffer::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_renderer_id);
    glViewport(0,
        0,
        static_cast<GLsizei>(m_specification.width),
        static_cast<GLsizei>(m_specification.height));
}

void OpenGLFramebuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void OpenGLFramebuffer::resize(const uint32_t width, const uint32_t height)
{
    if (width == 0 || height == 0 || width > s_max_framebuffer_size
        || height > s_max_framebuffer_size)
    {
        PINE_CORE_WARN("Attempted to resize framebuffer to {0}, {1}",
            width,
            height);
        return;
    }

    m_specification.width = width;
    m_specification.height = height;

    invalidate();
}

} // namespace pine
