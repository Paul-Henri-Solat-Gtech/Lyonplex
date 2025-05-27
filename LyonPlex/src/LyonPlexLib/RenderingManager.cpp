#include "pch.h"
#include "RenderingManager.h"

bool RenderingManager::Init()
{
    if (!m_graphicsDevice.Init(m_windowWP)) 
    {
        return false;
    }
    m_graphicsDevice.CreateGraphicsDevice();

    if (!m_descriptorManager.Init(&m_graphicsDevice))
    {
        return false;
    }
    m_descriptorManager.CreateDescriptorManager();

    if (!m_commandManager.Init(&m_graphicsDevice, &m_descriptorManager))
    {
        return false;
    }
    m_commandManager.CreateCommandManager();

    if (!m_render3D.Init(m_windowWP, &m_graphicsDevice, &m_descriptorManager, &m_commandManager)) 
    {
        return false;
    }
    m_render3D.CreatePipeline();

    return true;
}

void RenderingManager::RecordCommands()
{
    m_render3D.RecordCommands();
}

void RenderingManager::ExecuteCommands()
{
    ID3D12CommandList* cmdLists[] = { m_commandManager.GetCommandList().Get() };   
    m_graphicsDevice.GetCommandQueue()->ExecuteCommandLists(1, cmdLists);
}

void RenderingManager::Present()
{
    m_graphicsDevice.GetSwapChain()->Present(1, 0);
    m_graphicsDevice.GetFrameIndex() = m_graphicsDevice.GetSwapChain()->GetCurrentBackBufferIndex();
}

void RenderingManager::SignalAndWait()
{
    m_commandManager.GetFenceValue()++;
    m_graphicsDevice.GetCommandQueue()->Signal(m_commandManager.GetFence().Get(), m_commandManager.GetFenceValue());
    if (m_commandManager.GetFence()->GetCompletedValue() < m_commandManager.GetFenceValue()) {
        m_commandManager.GetFence()->SetEventOnCompletion(m_commandManager.GetFenceValue(), m_commandManager.GetFenceEvent());
        WaitForSingleObject(m_commandManager.GetFenceEvent(), INFINITE);
    }
}

