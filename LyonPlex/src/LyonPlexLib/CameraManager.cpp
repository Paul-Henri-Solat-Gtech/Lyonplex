#include "pch.h"
#include "CameraManager.h"

bool CameraManager::Init()
{
    m_position = { 0.f,0.f,0.f };
    m_right = { 1.f,0.f,0.f };
    m_up = { 0.f,1.f,0.f };
    m_forward = { 0.f,0.f,1.f };

    m_viewNeedsUpdate = true;

    return true;
}

void CameraManager::UpdateViewMatrix()
{
    if (!m_viewNeedsUpdate) return;

    XMVECTOR pos = XMLoadFloat3(&m_position);
    XMVECTOR forward = XMVector3Normalize(XMLoadFloat3(&m_forward));
    XMVECTOR up = XMVector3Normalize(XMLoadFloat3(&m_up));

    XMMATRIX view = XMMatrixLookToLH(pos, forward, up);
    XMStoreFloat4x4(&m_viewMatrix, view);

    m_viewNeedsUpdate = false;
}

void CameraManager::UpdateProjectionMatrix(float fovDegrees, float aspectRatio, float nearPlane, float farPlane)
{
    float fovRadians = XMConvertToRadians(fovDegrees);
    XMMATRIX proj = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearPlane, farPlane);
    XMStoreFloat4x4(&m_projectionMatrix, proj);
}

CameraBuffer CameraManager::GetCameraBuffer() const
{
    // Charge la view et la projection
    XMMATRIX viewMat = XMLoadFloat4x4(&m_viewMatrix);
    XMMATRIX projMat = XMLoadFloat4x4(&m_projectionMatrix);

    // Transpose en place pour HLSL (column-major)
    viewMat = XMMatrixTranspose(viewMat);
    projMat = XMMatrixTranspose(projMat);

    return CameraBuffer{ viewMat, projMat };
}

void CameraManager::SetPosition(const XMFLOAT3& pos)
{
    m_position = pos;
    m_viewNeedsUpdate = true;
}

void CameraManager::SetDirection(const XMFLOAT3& forward)
{
    m_forward = forward;
    m_viewNeedsUpdate = true;
}

void CameraManager::MoveForward(float distance)
{
    XMVECTOR pos = XMLoadFloat3(&m_position);
    XMVECTOR dir = XMLoadFloat3(&m_forward);
    pos = XMVectorAdd(pos, XMVectorScale(dir, distance));
    XMStoreFloat3(&m_position, pos);
    m_viewNeedsUpdate = true;
}

void CameraManager::MoveRight(float distance)
{
    XMVECTOR pos = XMLoadFloat3(&m_position);
    XMVECTOR dir = XMLoadFloat3(&m_right);
    pos = XMVectorAdd(pos, XMVectorScale(dir, distance));
    XMStoreFloat3(&m_position, pos);
    m_viewNeedsUpdate = true;
}

void CameraManager::MoveUp(float distance)
{
    XMVECTOR pos = XMLoadFloat3(&m_position);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // vecteur vertical
    pos = XMVectorAdd(pos, XMVectorScale(up, distance));
    XMStoreFloat3(&m_position, pos);
    m_viewNeedsUpdate = true;
}