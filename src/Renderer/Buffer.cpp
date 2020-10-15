#include "Sennet/pch.hpp"
#include "Sennet/Renderer/Buffer.hpp"

#include "Sennet/Renderer/Renderer.hpp"
#include "Sennet/Renderer/RendererAPI.hpp"

#include "Sennet/Platform/OpenGL/OpenGLBuffer.hpp"

namespace Sennet
{

void BufferLayout::CalculateOffsetAndStride()
{
	uint32_t offset = 0;
	m_Stride = 0;
	for (auto& element : m_Elements)
	{
		element.Offset = offset;
		offset += element.Size;
		m_Stride += element.Size;
	}
}

Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
{
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None: 
			SN_CORE_ASSERT(false, "renderer_api::none is currently \
				not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexBuffer>(vertices, size);
	}
	
	SN_CORE_ASSERT(false, "Unknown renderer API.");
	return nullptr;
}

Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
{
	switch (Renderer::GetAPI())
	{
		case RendererAPI::API::None: 
			SN_CORE_ASSERT(false, "Renderer API none is currently \
				not supported!");
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLIndexBuffer>(indices, count);
	}
	
	SN_CORE_ASSERT(false, "Unknown renderer API.");
	return nullptr;
}

}
