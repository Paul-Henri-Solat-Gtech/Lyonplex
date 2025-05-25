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
    D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device));

    // Command queue
    D3D12_COMMAND_QUEUE_DESC cqDesc = {};
    device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&commandQueue));

    // Swap chain
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
        commandQueue.Get(), hwnd, &scDesc, nullptr, nullptr, &tempSwapChain);
    tempSwapChain.As(&swapChain);
    frameIndex = swapChain->GetCurrentBackBufferIndex();

    // RTV Heap
    D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
    rtvDesc.NumDescriptors = FrameCount;
    rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(&rtvHeap));
    rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // Render targets
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < FrameCount; i++) {
        swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i]));
        device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, rtvDescriptorSize);
    }

    // Allocator & command list
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
    commandList->Close();

    // Fence
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    CreatePipeline();
}


void RenderingManager::CreatePipeline()
{
    // 1) Root signature
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> signature, error;
    D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    device->CreateRootSignature(
        0,
        signature->GetBufferPointer(),
        signature->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature)
    );

    // 2) Compilation des shaders
    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> psBlob;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompileFromFile(L"../../../src/LyonPlexLib/LyonShader.hlsl", nullptr, nullptr,"VSMain", "vs_5_0", 0, 0, &vsBlob, &errorBlob);

    if (FAILED(hr)) {
        if (errorBlob) {
            // Affiche la raison exacte dans une MessageBox ou via Debug
            std::string msg((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize());
            MessageBoxA(nullptr, msg.c_str(), "Shader Compilation Error", MB_OK | MB_ICONERROR);
        }
        throw std::runtime_error("VS compilation failed");
    }

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
    psoDesc.pRootSignature = rootSignature.Get();
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

    device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));

    // 5) Vertex buffer (inchangé)
    Vertex triangle[] = {
        {{ 0.0f,  0.25f, 0.0f},{1,0,0,1}},
        {{ 0.25f,-0.25f, 0.0f},{0,1,0,1}},
        {{-0.25f,-0.25f, 0.0f},{0,0,1,1}},
    };
    const UINT vbSize = sizeof(triangle);
    CD3DX12_HEAP_PROPERTIES hp(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC rd = CD3DX12_RESOURCE_DESC::Buffer(vbSize);
    device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer));

    void* p;
    vertexBuffer->Map(0, nullptr, &p);
    memcpy(p, triangle, vbSize);
    vertexBuffer->Unmap(0, nullptr);

    vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
    vertexBufferView.SizeInBytes = vbSize;
    vertexBufferView.StrideInBytes = sizeof(Vertex);
}

void RenderingManager::RecordCommands()
{
    commandAllocator->Reset();
    commandList->Reset(commandAllocator.Get(), pipelineState.Get());

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->ResourceBarrier(1, &barrier);

    // Définir le viewport et le scissor
    D3D12_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = static_cast<float>(800);  // ta largeur
    viewport.Height = static_cast<float>(600);  // ta hauteur
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    D3D12_RECT scissor = { 0, 0, 800, 600 };

    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissor);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize);
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    commandList->SetGraphicsRootSignature(rootSignature.Get());
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    commandList->DrawInstanced(3, 1, 0, 0);

    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    commandList->ResourceBarrier(1, &barrier);
    commandList->Close();
}

void RenderingManager::ExecuteCommands()
{
    ID3D12CommandList* cmdLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(1, cmdLists);
}

void RenderingManager::Present()
{
    swapChain->Present(1, 0);
    frameIndex = swapChain->GetCurrentBackBufferIndex();
}

void RenderingManager::SignalAndWait()
{
    fenceValue++;
    commandQueue->Signal(fence.Get(), fenceValue);
    if (fence->GetCompletedValue() < fenceValue) {
        fence->SetEventOnCompletion(fenceValue, fenceEvent);
        WaitForSingleObject(fenceEvent, INFINITE);
    }
}

