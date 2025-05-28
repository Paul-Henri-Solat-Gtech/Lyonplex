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
    // Reset CmdAlloc et CmdList
    m_commandManager.GetCommandAllocator()->Reset();
    //mp_commandManager->GetCommandList()->Reset(mp_commandManager->GetCommandAllocator().Get(), m_graphicsPipeline.GetPipelineState().Get());
    m_commandManager.GetCommandList()->Reset(m_commandManager.GetCommandAllocator().Get(), nullptr);

    // Barrier pour faire la transition du back buffer de l'etat PRESENT a RENDER_TARGET
    CD3DX12_RESOURCE_BARRIER barrier; 
        
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_graphicsDevice.GetRenderTargetResource(m_graphicsDevice.GetFrameIndex()), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandManager.GetCommandList()->ResourceBarrier(1, &barrier);

    // On fixe le RTV sur la bonne frame 
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_descriptorManager.GetRtvHeap()->GetCPUDescriptorHandleForHeapStart(), m_graphicsDevice.GetFrameIndex(), m_descriptorManager.GetRtvDescriptorSize());

    // Classes Render
    m_render3D.RecordCommands();

    // Transition du back buffer de RENDER_TARGET a PRESENT pour la presentation
    barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_graphicsDevice.GetRenderTargetResource(m_graphicsDevice.GetFrameIndex()), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandManager.GetCommandList()->ResourceBarrier(1, &barrier);

    m_commandManager.GetCommandList()->Close();
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
    if (m_commandManager.GetFence()->GetCompletedValue() < m_commandManager.GetFenceValue()) 
    {
        m_commandManager.GetFence()->SetEventOnCompletion(m_commandManager.GetFenceValue(), m_commandManager.GetFenceEvent());
        WaitForSingleObject(m_commandManager.GetFenceEvent(), INFINITE);
    }
}

