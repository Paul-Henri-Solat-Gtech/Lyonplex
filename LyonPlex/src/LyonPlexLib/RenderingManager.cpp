#include "pch.h"
#include "RenderingManager.h"

struct Vertex {
    XMFLOAT3 position;
    XMFLOAT4 color;
};

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
    
    // Render targets
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_descriptorManager.GetRtvHeap()->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < m_graphicsDevice.GetFrameCount(); i++) {
        m_graphicsDevice.GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
        m_graphicsDevice.GetDevice()->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1,  m_descriptorManager.GetRtvDescriptorSize());
    }

    if (!m_commandManager.Init(&m_graphicsDevice, &m_descriptorManager))
    {
        return false;
    }
    m_commandManager.CreateCommandManager();

    CreatePipeline();

    return true;
}


void RenderingManager::CreatePipeline()
{
    // 1) Root signature
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> signature, error;
    D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    m_graphicsDevice.GetDevice()->CreateRootSignature(0,signature->GetBufferPointer(),signature->GetBufferSize(),IID_PPV_ARGS(&m_rootSignature));

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

    m_graphicsDevice.GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));

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
    m_graphicsDevice.GetDevice()->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd,
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

    m_commandManager.GetCommandAllocator()->Reset();
    m_commandManager.GetCommandList()->Reset(m_commandManager.GetCommandAllocator().Get(), m_pipelineState.Get());
    
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_renderTargets[m_graphicsDevice.GetFrameIndex()].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandManager.GetCommandList()->ResourceBarrier(1, &barrier);

    // Définir le viewport et le scissor
    D3D12_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = 800;  // window width
    viewport.Height = 600;  // window height
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    // Zone pour dessiner des pixels
    D3D12_RECT scissor = { 0, 0, 800, 600 };

    m_commandManager.GetCommandList()->RSSetViewports(1, &viewport);
    m_commandManager.GetCommandList()->RSSetScissorRects(1, &scissor);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle( m_descriptorManager.GetRtvHeap()->GetCPUDescriptorHandleForHeapStart(), m_graphicsDevice.GetFrameIndex(),  m_descriptorManager.GetRtvDescriptorSize());
    m_commandManager.GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_commandManager.GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    //Draw vertices (mesh)
    m_commandManager.GetCommandList()->SetGraphicsRootSignature(m_rootSignature.Get());
    m_commandManager.GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_commandManager.GetCommandList()->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    m_commandManager.GetCommandList()->DrawInstanced(3, 1, 0, 0);

    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_renderTargets[m_graphicsDevice.GetFrameIndex()].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
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
    if (m_commandManager.GetFence()->GetCompletedValue() < m_commandManager.GetFenceValue()) {
        m_commandManager.GetFence()->SetEventOnCompletion(m_commandManager.GetFenceValue(), m_commandManager.GetFenceEvent());
        WaitForSingleObject(m_commandManager.GetFenceEvent(), INFINITE);
    }
}

