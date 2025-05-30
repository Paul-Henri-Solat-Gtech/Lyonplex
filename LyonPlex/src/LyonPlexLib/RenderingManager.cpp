#include "pch.h"
#include "RenderingManager.h"

bool RenderingManager::Init()
{
    // Graphics Device
    if (!m_graphicsDevice.Init(m_windowWP)) 
    {
        return false;
    }
    m_graphicsDevice.CreateGraphicsDevice();

    // Descriptor Manager
    if (!m_descriptorManager.Init(&m_graphicsDevice))
    {
        return false;
    }
    m_descriptorManager.CreateDescriptorManager();

    m_graphicsDevice.SetRenderTargets(&m_descriptorManager);

    // Command Manager
    if (!m_commandManager.Init(&m_graphicsDevice, &m_descriptorManager))
    {
        return false;
    }
    m_commandManager.CreateCommandManager();

    // Render 3D
    if (!m_render3D.Init(m_windowWP, &m_graphicsDevice, &m_descriptorManager, &m_commandManager)) 
    {
        return false;
    }
    m_render3D.CreatePipeline();

    return true;
}

void RenderingManager::RecordCommands()
{
    m_commandManager.Begin();

    // Barrier pour faire la transition du back buffer de l'etat PRESENT a RENDER_TARGET
    CD3DX12_RESOURCE_BARRIER barrier;

    SetBarrierToRenderTarget(barrier);

    // Classes Render
    /**/

    m_render3D.RecordCommands();

    /**/

    SetBarrierToPresent(barrier);   

    m_commandManager.End();
}


void RenderingManager::SetBarrierToRenderTarget(CD3DX12_RESOURCE_BARRIER& barrier)
{
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_graphicsDevice.GetRenderTargetResource(m_graphicsDevice.GetFrameIndex()), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandManager.GetCommandList()->ResourceBarrier(1, &barrier);

    // On fixe le RTV sur la bonne frame 
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_descriptorManager.GetRtvHeap()->GetCPUDescriptorHandleForHeapStart(), m_graphicsDevice.GetFrameIndex(), m_descriptorManager.GetRtvDescriptorSize());
}
void RenderingManager::SetBarrierToPresent(CD3DX12_RESOURCE_BARRIER& barrier)
{
    // Transition du back buffer de RENDER_TARGET a PRESENT pour la presentation
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_graphicsDevice.GetRenderTargetResource(m_graphicsDevice.GetFrameIndex()), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandManager.GetCommandList()->ResourceBarrier(1, &barrier);
}



void RenderingManager::ExecuteCommands()
{
    m_commandManager.ExecuteCmdLists();
}

void RenderingManager::Present()
{
    m_graphicsDevice.Present();
}

void RenderingManager::SynchroGPUCPU()
{
    m_commandManager.SignalAndWait();
}

void RenderingManager::Release()
{
    m_graphicsDevice.Release();
    m_descriptorManager.Release();
    m_commandManager.Release();
    m_render3D.Release();
}

void RenderingManager::UpdateTemp()
{
    m_render3D.UpdateTemp();
}

