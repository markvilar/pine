#include "Sennet/Renderer/OrthographicCamera.hpp"
#include "Sennet/Pch.hpp"

namespace Sennet
{

OrthographicCamera::OrthographicCamera(
    float left, float right, float bottom, float top)
    : m_ProjectionMatrix(Ortho(left, right, bottom, top, -1.0f, 1.0f)),
      m_ViewMatrix(1.0f)
{
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicCamera::SetProjection(
    float left, float right, float bottom, float top)
{
    m_ProjectionMatrix = Ortho(left, right, bottom, top, -1.0f, 1.0f);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void OrthographicCamera::RecalculateViewMatrix()
{
    Mat4 transform = Translate(Mat4(1.0f), m_Position)
        * Rotate(Mat4(1.0f), m_Rotation, Vec3(0, 0, 1));

    m_ViewMatrix = Inverse(transform);
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

} // namespace Sennet
