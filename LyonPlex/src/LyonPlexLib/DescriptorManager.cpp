#include "pch.h"
#include "DescriptorManager.h"

bool DescriptorManager::Init(GraphicsDevice* graphicsDevice)
{
    mp_graphicsDevice = graphicsDevice;

    return true;
}

void DescriptorManager::CreateDescriptorManager()
{
    CreateRTVHeap();
}

void DescriptorManager::CreateRTVHeap()
{
    // RTV Heap
    D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
    rtvDesc.NumDescriptors = mp_graphicsDevice->GetFrameCount();
    rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    mp_graphicsDevice->GetDevice()->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&m_rtvHeap));
    m_rtvDescriptorSize = mp_graphicsDevice->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void DescriptorManager::Release()
{

}
