#pragma once

#include "Sennet/Renderer/Texture.hpp"

#include "Sennet/Primitives/Image.hpp"

namespace Sennet
{

class OpenGLTexture2D : public Texture2D
{
public:
	OpenGLTexture2D(const std::string& path);
	OpenGLTexture2D(const Image& image);
	~OpenGLTexture2D();

	virtual uint32_t GetWidth() const override { return m_Width; }
	virtual uint32_t GetHeight() const override { return m_Height; }

	virtual uint32_t GetRendererID() const override { return m_RendererID; }

	virtual void Bind(uint32_t slot = 0) const override;

private:
	std::string m_Source;
	uint32_t m_Width;
	uint32_t m_Height;
	uint32_t m_RendererID;
};

}
