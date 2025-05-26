#include "pch.h"
#include "RenderingManager.h"

struct Vertex {
    XMFLOAT3 position;
    XMFLOAT4 color;
};

void RenderingManager::Init()
{
    // Créer le device
    ComPtr<IDXGIFactory6> factory;
    CreateDXGIFactory1(IID_PPV_ARGS(&factory));

    ComPtr<IDXGIAdapter1> adapter;
    factory->EnumAdapters1(0, &adapter);
    D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));

    // Créer la Command queue
    D3D12_COMMAND_QUEUE_DESC cqDesc = {};
    m_device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&m_commandQueue));

    // Créer le Swap chain
    DXGI_SWAP_CHAIN_DESC1 scDesc = {};
    scDesc.BufferCount = FrameCount;
    scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scDesc.Width = 800;  // ou la taille de ta fenêtre
    scDesc.Height = 600;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> tempSwapChain;
    factory->CreateSwapChainForHwnd(
        m_commandQueue.Get(), m_createdWindow, &scDesc, nullptr, nullptr, &tempSwapChain);
    tempSwapChain.As(&m_swapChain);
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // RTV Heap
    D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
    rtvDesc.NumDescriptors = FrameCount;
    rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    m_device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&m_rtvHeap));
    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // Render targets
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < FrameCount; i++) {
        m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
        m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, m_rtvDescriptorSize);
    }

    // Allocator & command list
    m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
    m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));
    m_commandList->Close();

    // Fence
    m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    CreatePipeline();
}


void RenderingManager::CreatePipeline()
{
    // 1) Root signature
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> signature, error;
    D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    m_device->CreateRootSignature(0,signature->GetBufferPointer(),signature->GetBufferSize(),IID_PPV_ARGS(&m_rootSignature));

    // 2) Compilation des shaders
    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> psBlob;
    ComPtr<ID3DBlob> errorBlob;

    //VERTEX SHADER
    HRESULT hr = D3DCompileFromFile(L"../../../src/LyonPlexLib/LyonShader.hlsl", nullptr, nullptr,"VSMain", "vs_5_0", 0, 0, &vsBlob, &errorBlob);

    if (FAILED(hr)) {
        if (errorBlob) {
            // Affiche la raison exacte dans une MessageBox ou via Debug
            std::string msg((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize());
            MessageBoxA(nullptr, msg.c_str(), "Shader Compilation Error", MB_OK | MB_ICONERROR);
        }
        throw std::runtime_error("VS compilation failed");
    }

    //PIXEL SHADER
    hr = D3DCompileFromFile( L"../../../src/LyonPlexLib/LyonShader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &psBlob, &errorBlob);

    if (FAILED(hr)) {
        if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        throw std::runtime_error("PS compilation failed");
    }

    // 3) Input layout
    D3D12_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };

    // 4) PSO
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { layout, _countof(layout) };
    psoDesc.pRootSignature = m_rootSignature.Get();
    psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
    psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState.DepthEnable = FALSE;
    psoDesc.DepthStencilState.StencilEnable = FALSE;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;

    m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));

    // 5) Vertex buffer (inchangé)
    // Draw a trinagle
    Vertex triangle[] = {
        {{ 0.0f,  0.25f, 0.0f},{1,0,0,1}},
        {{ 0.25f,-0.25f, 0.0f},{0,1,0,1}},
        {{-0.25f,-0.25f, 0.0f},{0,0,1,1}},
    };
    const UINT vbSize = sizeof(triangle);
    CD3DX12_HEAP_PROPERTIES hp(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC rd = CD3DX12_RESOURCE_DESC::Buffer(vbSize);
    m_device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vertexBuffer));

    void* p;
    m_vertexBuffer->Map(0, nullptr, &p);
    memcpy(p, triangle, vbSize);
    m_vertexBuffer->Unmap(0, nullptr);

    m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.SizeInBytes = vbSize;
    m_vertexBufferView.StrideInBytes = sizeof(Vertex);
}

void RenderingManager::RecordCommands()
{
    m_commandAllocator->Reset();
    m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get());

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandList->ResourceBarrier(1, &barrier);

    // Définir le viewport et le scissor
    D3D12_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(800);  // window width
    viewport.Height = static_cast<float>(600);  // window height
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    // Zone pour dessiner des pixels
    D3D12_RECT scissor = { 0, 0, 800, 600 };

    m_commandList->RSSetViewports(1, &viewport);
    m_commandList->RSSetScissorRects(1, &scissor);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    //Draw vertices (mesh)
    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    m_commandList->DrawInstanced(3, 1, 0, 0);

    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1, &barrier);
    m_commandList->Close();
}

void RenderingManager::ExecuteCommands()
{
    ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(1, cmdLists);
}

void RenderingManager::Present()
{
    m_swapChain->Present(1, 0);
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void RenderingManager::SignalAndWait()
{
    m_fenceValue++;
    m_commandQueue->Signal(m_fence.Get(), m_fenceValue);
    if (m_fence->GetCompletedValue() < m_fenceValue) {
        m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

