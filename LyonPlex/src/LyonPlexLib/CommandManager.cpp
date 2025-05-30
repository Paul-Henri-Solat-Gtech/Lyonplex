#include "pch.h"
#include "CommandManager.h"

bool CommandManager::Init(GraphicsDevice* graphicsDevice, DescriptorManager* descriptorManager)
{
    mp_graphicsDevice = graphicsDevice;
    mp_descriptorManager = descriptorManager;


    return true;
}

void CommandManager::CreateCommandManager()
{
    CreateCommandAllocator();
    CreateCommandList();
    CreateFence();
}

void CommandManager::Begin()
{
    // Reset CmdAlloc et CmdList
    m_commandAllocator.Get()->Reset();
    m_commandList.Get()->Reset(m_commandAllocator.Get(), nullptr);
}

void CommandManager::End()
{
    m_commandList->Close();
}

void CommandManager::ExecuteCmdLists()
{
    ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
    mp_graphicsDevice->GetCommandQueue()->ExecuteCommandLists(1, cmdLists);
}

void CommandManager::SignalAndWait()
{
    m_fenceValue++;
    mp_graphicsDevice->GetCommandQueue()->Signal(m_fence.Get(), m_fenceValue);
    if (m_fence->GetCompletedValue() < m_fenceValue)
    {
        m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

void CommandManager::CreateCommandAllocator()
{
    // Allocator
    mp_graphicsDevice->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
}

void CommandManager::CreateCommandList()
{
    // command list
    mp_graphicsDevice->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));
    m_commandList->Close();
}

void CommandManager::CreateFence()
{
    // Fence
    mp_graphicsDevice->GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void CommandManager::Release()
{
    mp_graphicsDevice = nullptr;
    delete mp_graphicsDevice;

    mp_descriptorManager = nullptr;
    delete mp_descriptorManager;
}
