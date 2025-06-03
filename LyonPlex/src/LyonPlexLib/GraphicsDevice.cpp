#include "pch.h"
#include "GraphicsDevice.h"

bool GraphicsDevice::Init(HWND windowWP)
{
    m_windowWP = windowWP;

    m_renderTargets.resize(FRAMECOUNT);

    return true;
}

void GraphicsDevice::CreateGraphicsDevice()
{
    CreateDevice();
    CreateCommandQueue();
    CreateSwapChain();
}

void GraphicsDevice::CreateDevice()
{
    // Creer le device
    //ComPtr<IDXGIFactory6> factory;
    CreateDXGIFactory1(IID_PPV_ARGS(&m_factory));

    ComPtr<IDXGIAdapter1> adapter;
    m_factory->EnumAdapters1(0, &adapter);
    D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
}

void GraphicsDevice::CreateCommandQueue()
{
    // Creer la Command queue
    D3D12_COMMAND_QUEUE_DESC cqDesc = {};
    m_device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&m_commandQueue));
}

void GraphicsDevice::CreateSwapChain()
{
    // Creer le Swap chain
    DXGI_SWAP_CHAIN_DESC1 scDesc = {};
    scDesc.BufferCount = FRAMECOUNT;
    scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.Width = 800;  // ou la taille de ta fenetre
    scDesc.Height = 600;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> tempSwapChain;
    m_factory->CreateSwapChainForHwnd(
        m_commandQueue.Get(), m_windowWP, &scDesc, nullptr, nullptr, &tempSwapChain);
    tempSwapChain.As(&m_swapChain);
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void GraphicsDevice::SetRenderTargets(DescriptorManager* descManager)
{ 
    m_renderTargets.resize(FRAMECOUNT);

    for (UINT i = 0; i < FRAMECOUNT; i++)
    {
        m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));

        // alloue le prochain descriptor RTV dans le heap
        D3D12_CPU_DESCRIPTOR_HANDLE RtvHandle = descManager->AllocateRtvCPU();

        m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, RtvHandle);
        m_rtvHandle.Offset(1, descManager->GetRtvDescriptorSize());
    }
}


void GraphicsDevice::Present()
{
    m_swapChain->Present(1, 0);
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void GraphicsDevice::Release()
{

}
