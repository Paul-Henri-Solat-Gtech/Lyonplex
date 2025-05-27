#include "pch.h"
#include "Render3D.h"

bool Render3D::Init(HWND windowHandle, GraphicsDevice* graphicsDevice, DescriptorManager* descriptorManager, CommandManager* commandManager)
{
	mp_graphicsDevice = graphicsDevice;
	mp_descriptorManager = descriptorManager;
	mp_commandManager = commandManager;
	m_windowWP = windowHandle;

	m_graphicsPipeline.Init(mp_graphicsDevice, mp_descriptorManager, mp_commandManager);
    
    // Render targets
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mp_descriptorManager->GetRtvHeap()->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < mp_graphicsDevice->GetFrameCount(); i++) {
        mp_graphicsDevice->GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]));
        mp_graphicsDevice->GetDevice()->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, mp_descriptorManager->GetRtvDescriptorSize());
    }

	return true;
}

void Render3D::Resize(int w, int h)
{
	int x = 3;
}

void Render3D::RecordCommands()
{
    mp_commandManager->GetCommandAllocator()->Reset();
    mp_commandManager->GetCommandList()->Reset(mp_commandManager->GetCommandAllocator().Get(), GetGraphicsPipeline().GetPipelineState().Get());

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_renderTargets[mp_graphicsDevice->GetFrameIndex()].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    mp_commandManager->GetCommandList()->ResourceBarrier(1, &barrier);

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

    mp_commandManager->GetCommandList()->RSSetViewports(1, &viewport);
    mp_commandManager->GetCommandList()->RSSetScissorRects(1, &scissor);

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mp_descriptorManager->GetRtvHeap()->GetCPUDescriptorHandleForHeapStart(), mp_graphicsDevice->GetFrameIndex(), mp_descriptorManager->GetRtvDescriptorSize());
    mp_commandManager->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    mp_commandManager->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    //Draw vertices (mesh)
    mp_commandManager->GetCommandList()->SetGraphicsRootSignature(m_graphicsPipeline.GetRootSignature().Get());
    mp_commandManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    mp_commandManager->GetCommandList()->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    mp_commandManager->GetCommandList()->DrawInstanced(3, 1, 0, 0);

    barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_renderTargets[mp_graphicsDevice->GetFrameIndex()].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    mp_commandManager->GetCommandList()->ResourceBarrier(1, &barrier);
    mp_commandManager->GetCommandList()->Close();
}

void Render3D::CreatePipeline()
{
	m_graphicsPipeline.CreatePipeline();

    //--------------------------------------------------- Draw a triangle
    Vertex triangle[] = {
        {{ 0.0f,  0.25f, 0.0f},{1,0,0,1}},
        {{ 0.25f,-0.25f, 0.0f},{0,1,0,1}},
        {{-0.25f,-0.25f, 0.0f},{0,0,1,1}},
    };
    const UINT vbSize = sizeof(triangle);
    CD3DX12_HEAP_PROPERTIES hp(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC rd = CD3DX12_RESOURCE_DESC::Buffer(vbSize);
    mp_graphicsDevice->GetDevice()->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vertexBuffer));

    void* p;
    m_vertexBuffer->Map(0, nullptr, &p);
    memcpy(p, triangle, vbSize);
    m_vertexBuffer->Unmap(0, nullptr);

    m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
    m_vertexBufferView.SizeInBytes = vbSize;
    m_vertexBufferView.StrideInBytes = sizeof(Vertex);
    //----------------------------------------------------- Pour test

}