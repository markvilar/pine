#include "Pine/Renderer/Renderer.hpp"

#include "Pine/Pch.hpp"
#include "Pine/Platform/OpenGL/OpenGLShader.hpp"
#include "Pine/Renderer/RenderCommand.hpp"
#include "Pine/Renderer/Renderer2D.hpp"

namespace Pine
{

Scope<Renderer::SceneData> Renderer::s_SceneData =
    CreateScope<Renderer::SceneData>();

struct Data
{
    Ref<ShaderLibrary> Library;
    Ref<Texture2D> BlackTexture;
    Ref<Texture2D> WhiteTexture;
};

static Data* s_Data = nullptr;

void Renderer::Init()
{
    s_Data = new Data();

    // Initialize shader library and load shaders.
    s_Data->Library = CreateRef<ShaderLibrary>();

    // TODO: Fix working directory and shader paths.
    Renderer::GetShaderLibrary()->Load("resources/shaders/Renderer2D.glsl");

    constexpr uint32_t width = 1;
    constexpr uint32_t height = 1;
    constexpr std::array<uint8_t, 4> whiteColor = {255, 255, 255, 255};
    constexpr std::array<uint8_t, 4> blackColor = {0, 0, 0, 255};

    Image whiteImage(whiteColor.data(), width, height, ImageFormat::RGBA);
    Image blackImage(blackColor.data(), width, height, ImageFormat::RGBA);

    s_Data->WhiteTexture = Texture2D::Create(whiteImage);
    s_Data->BlackTexture = Texture2D::Create(blackImage);

    RenderCommand::Init();
    Renderer2D::Init();
}

void Renderer::OnWindowResize(const uint32_t width, const uint32_t height)
{
    RenderCommand::SetViewport(0, 0, width, height);
}

Ref<ShaderLibrary> Renderer::GetShaderLibrary()
{
    PINE_CORE_ASSERT(s_Data != nullptr, "Renderer un-initialized.");
    return s_Data->Library;
}

Ref<Texture2D> Renderer::GetBlackTexture()
{
    PINE_CORE_ASSERT(s_Data != nullptr, "Renderer un-initialized.");
    return s_Data->BlackTexture;
}

Ref<Texture2D> Renderer::GetWhiteTexture()
{
    PINE_CORE_ASSERT(s_Data != nullptr, "Renderer un-initialized.");
    return s_Data->WhiteTexture;
}

void Renderer::BeginScene(const OrthographicCamera& camera)
{
    s_SceneData->ViewProjectionMatrix = camera.GetViewProjectionMatrix();
}

void Renderer::EndScene() {}

void Renderer::Submit(const Ref<Shader>& shader,
    const Ref<VertexArray>& vertexArray, const Mat4& transform)
{
    shader->Bind();
    shader->SetMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
    shader->SetMat4("u_Transform", transform);

    vertexArray->Bind();
    RenderCommand::DrawIndexed(vertexArray);
}

} // namespace Pine