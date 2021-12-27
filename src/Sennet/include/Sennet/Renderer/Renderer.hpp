#pragma once

#include "Sennet/Core/Base.hpp"

#include "Sennet/Renderer/OrthographicCamera.hpp"
#include "Sennet/Renderer/RendererAPI.hpp"
#include "Sennet/Renderer/Shader.hpp"
#include "Sennet/Renderer/Texture.hpp"

#include "Sennet/Utils/Math.hpp"

namespace Sennet
{

class ShaderLibrary;

class Renderer
{
public:
    static void Init();
    static void OnWindowResize(const uint32_t width, const uint32_t height);

    static Ref<ShaderLibrary> GetShaderLibrary();

    static Ref<Texture2D> GetBlackTexture();
    static Ref<Texture2D> GetWhiteTexture();

    static void BeginScene(const OrthographicCamera& camera);
    static void EndScene();

    static void Submit(const Ref<Shader>& shader,
        const Ref<VertexArray>& vertexArray,
        const Mat4& transform = Mat4(1.0f));

    inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

private:
    struct SceneData
    {
        glm::mat4 ViewProjectionMatrix;
    };

    static Scope<SceneData> s_SceneData;
};

} // namespace Sennet
