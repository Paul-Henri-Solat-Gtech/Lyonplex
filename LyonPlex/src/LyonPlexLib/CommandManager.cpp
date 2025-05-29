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

void CommandManager::ResetCommands()
{
    // Reset CmdAlloc et CmdList
    m_commandAllocator.Get()->Reset();
    m_commandList.Get()->Reset(m_commandAllocator.Get(), nullptr);
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

}
