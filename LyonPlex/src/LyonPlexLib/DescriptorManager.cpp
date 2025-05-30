#include "pch.h"
#include "DescriptorManager.h"

bool DescriptorManager::Init(GraphicsDevice* graphicsDevice)
{
    mp_graphicsDevice = graphicsDevice;

    return true;
}

void DescriptorManager::CreateDescriptorManager()
{
    CreateRtvHeap(mp_graphicsDevice->GetFrameCount());
}

void DescriptorManager::CreateHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool shaderVisible, ComPtr<ID3D12DescriptorHeap>& heap, UINT& descriptorSize, UINT& nextOffset)
{
    D3D12_DESCRIPTOR_HEAP_DESC desc = {};
    desc.Type = type;
    desc.NumDescriptors = numDescriptors;
    desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    mp_graphicsDevice->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heap));
    descriptorSize = mp_graphicsDevice->GetDevice()->GetDescriptorHandleIncrementSize(type);
    nextOffset = 0;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorManager::AllocateCPU(ComPtr<ID3D12DescriptorHeap>& heap, UINT descriptorSize, UINT& nextOffset)
{
    auto handle = heap->GetCPUDescriptorHandleForHeapStart();
    handle.ptr += nextOffset++ * descriptorSize;
    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorManager::AllocateGPU(ComPtr<ID3D12DescriptorHeap>& heap, UINT descriptorSize, UINT& nextOffset)
{
    auto handle = heap->GetGPUDescriptorHandleForHeapStart();
    handle.ptr += (nextOffset - 1) * descriptorSize;
    return handle;
}

void DescriptorManager::CreateSrvHeap(UINT numDescriptors)
{
    CreateHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, numDescriptors, true, m_srvHeap, m_srvDescriptorSize, m_srvNextOffset);
}
void DescriptorManager::CreateRtvHeap(UINT numDescriptors)
{
    CreateHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, numDescriptors, false, m_rtvHeap, m_rtvDescriptorSize, m_rtvNextOffset);
}
void DescriptorManager::CreateDsvHeap(UINT numDescriptors)
{
    CreateHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, numDescriptors, false, m_dsvHeap, m_dsvDescriptorSize, m_dsvNextOffset);
}
void DescriptorManager::CreateSamplerHeap(UINT numDescriptors)
{
    CreateHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, numDescriptors, true, m_samplerHeap, m_samplerDescriptorSize, m_samplerNextOffset);
}

void DescriptorManager::Release()
{
    delete mp_graphicsDevice;
}
