#include "pch.h"
#include "CameraManager.h"

bool CameraManager::Init()
{
    m_position = { 0.f,0.f,0.f };
    m_right = { 1.f,0.f,0.f };
    m_up = { 0.f,1.f,0.f };
    m_forward = { 0.f,0.f,1.f };

    return true;
}
