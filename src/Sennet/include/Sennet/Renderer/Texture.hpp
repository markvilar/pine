#pragma once

#include <filesystem>

#include "Sennet/Core/Base.hpp"
#include "Sennet/Renderer/Image.hpp"
#include "Sennet/Renderer/RendererAPI.hpp"

namespace Sennet
{

class Texture
{
public:
    virtual ~Texture() = default;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    virtual RendererID GetRendererID() const = 0;

    virtual void Bind(const uint32_t slot = 0) const = 0;
    virtual void Unbind() const = 0;

    virtual bool operator==(const Texture& other) const = 0;
};

class Texture2D : public Texture
{
public:
    static Ref<Texture2D> Create(const std::filesystem::path& filePath);
    static Ref<Texture2D> Create(const Image& image);
};

} // namespace Sennet
