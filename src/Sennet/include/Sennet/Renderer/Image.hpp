#pragma once

#include <filesystem>
#include <vector>

namespace Sennet
{

enum class TextureFormat : uint8_t
{
    RED,
    RG,
    RGB,
    BGR,
    RGBA,
    BGRA,
    DEPTH
};

enum class ImageFileFormat : uint8_t
{
    UNKNOWN,
    JPG,
    PNG,
    BMP,
    TGA
};

enum class ImageFormat : uint8_t
{
    GRAY = static_cast<uint8_t>(TextureFormat::RED),
    GRAY_ALPHA = static_cast<uint8_t>(TextureFormat::RG),
    RGB = static_cast<uint8_t>(TextureFormat::RGB),
    BGR = static_cast<uint8_t>(TextureFormat::BGR),
    RGBA = static_cast<uint8_t>(TextureFormat::RGBA),
    BGRA = static_cast<uint8_t>(TextureFormat::BGRA),
    DEPTH = static_cast<uint8_t>(TextureFormat::DEPTH)
};

struct Image
{
    uint32_t Width;
    uint32_t Height;
    uint8_t Channels;
    ImageFormat Format;

    const std::vector<uint8_t> Buffer;

public:
    Image() = default;
    Image(const Image& image) = default;
    Image(Image&& image) = default;
    Image(const uint8_t* data, const uint32_t width, const uint32_t height,
        const uint8_t channels, const ImageFormat format);
    ~Image() = default;
};

const Image ReadImage(
    const std::filesystem::path& filepath, const bool flip = false);

bool WriteImage(const std::filesystem::path& filepath, const Image& image,
    const bool flip = false);

} // namespace Sennet