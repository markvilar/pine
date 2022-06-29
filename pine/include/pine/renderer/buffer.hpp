#pragma once

#include <memory>
#include <string>
#include <vector>

#include "pine/core/common.hpp"

namespace pine
{

enum class ShaderDataType
{
    None = 0,
    Float,
    Float2,
    Float3,
    Float4,
    Mat3,
    Mat4,
    Int,
    Int2,
    Int3,
    Int4,
    Uint,
    Uint2,
    Uint3,
    Uint4,
    Bool
};

static constexpr uint32_t ShaderDataTypeSize(const ShaderDataType type)
{
    switch (type)
    {
    case ShaderDataType::Float:
        return 4;
    case ShaderDataType::Float2:
        return 4 * 2;
    case ShaderDataType::Float3:
        return 4 * 3;
    case ShaderDataType::Float4:
        return 4 * 4;
    case ShaderDataType::Mat3:
        return 4 * 3 * 3;
    case ShaderDataType::Mat4:
        return 4 * 4 * 4;
    case ShaderDataType::Int:
        return 4;
    case ShaderDataType::Int2:
        return 4 * 2;
    case ShaderDataType::Int3:
        return 4 * 3;
    case ShaderDataType::Int4:
        return 4 * 4;
    case ShaderDataType::Uint:
        return 4;
    case ShaderDataType::Uint2:
        return 4 * 2;
    case ShaderDataType::Uint3:
        return 4 * 3;
    case ShaderDataType::Uint4:
        return 4 * 4;
    case ShaderDataType::Bool:
        return 1;
    case ShaderDataType::None:
        return 0;
    }

    PINE_CORE_ASSERT(false, "Unknown ShaderDataType!");
    return 0;
}

static constexpr uint32_t ShaderDataTypeComponentCount(
    const ShaderDataType type)
{
    switch (type)
    {
    case ShaderDataType::Float:
        return 1;
    case ShaderDataType::Float2:
        return 2;
    case ShaderDataType::Float3:
        return 3;
    case ShaderDataType::Float4:
        return 4;
    case ShaderDataType::Mat3:
        return 3 * 3;
    case ShaderDataType::Mat4:
        return 4 * 4;
    case ShaderDataType::Int:
        return 1;
    case ShaderDataType::Int2:
        return 2;
    case ShaderDataType::Int3:
        return 3;
    case ShaderDataType::Int4:
        return 4;
    case ShaderDataType::Uint:
        return 1;
    case ShaderDataType::Uint2:
        return 2;
    case ShaderDataType::Uint3:
        return 3;
    case ShaderDataType::Uint4:
        return 4;
    case ShaderDataType::Bool:
        return 1;
    case ShaderDataType::None:
        return 0;
    }

    PINE_CORE_ASSERT(false, "Unknown ShaderDataType!");
    return 0;
}

struct BufferElement
{
    std::string Name = "";
    ShaderDataType Type = ShaderDataType::None;
    uint32_t Offset = 0;
    uint32_t Size = 0;
    bool Normalized = false;

    BufferElement() = default;
    BufferElement(const BufferElement&) = default;
    BufferElement(BufferElement&&) = default;
    ~BufferElement() = default;

    BufferElement(const ShaderDataType type, const std::string& name,
        const bool normalized = false)
        : Type(type), Name(name), Size(ShaderDataTypeSize(type)), Offset(0),
          Normalized(normalized)
    {
    }

    BufferElement& operator=(const BufferElement&) = default;
    BufferElement& operator=(BufferElement&&) = default;

    uint32_t GetComponentCount() const
    {
        return ShaderDataTypeComponentCount(Type);
    }
};

class BufferLayout
{
public:
    BufferLayout() = default;
    BufferLayout(const BufferLayout&) = default;
    BufferLayout(BufferLayout&&) = default;
    ~BufferLayout() = default;

    BufferLayout(const std::initializer_list<BufferElement>& elements)
        : m_Elements(elements)
    {
        CalculateOffsetAndStride();
    }

    BufferLayout& operator=(const BufferLayout&) = default;
    BufferLayout& operator=(BufferLayout&&) = default;

    inline uint32_t GetStride() const { return m_Stride; }
    inline const std::vector<BufferElement>& GetElements() const
    {
        return m_Elements;
    }

    std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
    std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
    std::vector<BufferElement>::const_iterator begin() const
    {
        return m_Elements.begin();
    }
    std::vector<BufferElement>::const_iterator end() const
    {
        return m_Elements.end();
    }

private:
    void CalculateOffsetAndStride();

private:
    std::vector<BufferElement> m_Elements;
    uint32_t m_Stride = 0;
};

class VertexBuffer
{
public:
    virtual ~VertexBuffer() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void SetData(const void* data, const uint32_t size) = 0;

    virtual const BufferLayout& GetLayout() const = 0;
    virtual void SetLayout(const BufferLayout& layout) = 0;

    static std::unique_ptr<VertexBuffer> Create(const uint32_t size);
    static std::unique_ptr<VertexBuffer> Create(const float* vertices,
        const uint32_t size);
};

class IndexBuffer
{
public:
    virtual ~IndexBuffer() = default;

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual uint32_t GetCount() const = 0;

    static std::unique_ptr<IndexBuffer> Create(const uint32_t* indices,
        const uint32_t count);
};

} // namespace pine