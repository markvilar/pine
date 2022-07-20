#pragma once

#include "pine/renderer/buffer.hpp"
#include "pine/renderer/renderer_api.hpp"

namespace pine
{

class OpenGLVertexBuffer : public VertexBuffer
{
public:
    OpenGLVertexBuffer(const uint32_t size);
    OpenGLVertexBuffer(const float* vertices, const uint32_t size);
    virtual ~OpenGLVertexBuffer();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void SetData(const void* data, const uint32_t size) override;

    virtual const BufferLayout& GetLayout() const override { return m_Layout; }
    virtual void SetLayout(const BufferLayout& layout) override
    {
        m_Layout = layout;
    }

private:
    RendererID m_RendererID;
    BufferLayout m_Layout;
};

class OpenGLIndexBuffer : public IndexBuffer
{
public:
    OpenGLIndexBuffer(const uint32_t* indices, const uint32_t count);
    virtual ~OpenGLIndexBuffer();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual uint32_t GetCount() const override { return m_Count; }

private:
    RendererID m_RendererID;
    uint32_t m_Count;
};

class OpenGLVertexArray : public VertexArray
{
public:
    OpenGLVertexArray();
    virtual ~OpenGLVertexArray();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void SetVertexBuffer(std::unique_ptr<VertexBuffer> buffer) override;
    virtual void SetIndexBuffer(std::unique_ptr<IndexBuffer> buffer) override;

    virtual VertexBuffer& GetVertexBuffer() const override
    {
        return *m_VertexBuffer.get();
    }

    virtual IndexBuffer& GetIndexBuffer() const override
    {
        return *m_IndexBuffer.get();
    }

private:
    RendererID m_RendererID = {};
    std::unique_ptr<VertexBuffer> m_VertexBuffer = {};
    std::unique_ptr<IndexBuffer> m_IndexBuffer = {};
};

} // namespace pine
