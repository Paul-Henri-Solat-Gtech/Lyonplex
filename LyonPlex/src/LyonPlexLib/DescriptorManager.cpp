#include "pch.h"
#include "DescriptorManager.h"

bool DescriptorManager::Init(GraphicsDevice* graphicsDevice)
{
	mp_graphicsDevice = graphicsDevice;

	return true;
}

void DescriptorManager::CreateDescriptors()
{
	m_depthResources.resize(mp_graphicsDevice->GetFrameCount());


	CreateRtvHeap(mp_graphicsDevice->GetFrameCount());
	CreateDsvHeap(mp_graphicsDevice->GetFrameCount());

	for (int i = 0; i < mp_graphicsDevice->GetFrameCount(); i++)
	{
		CreateDepthStencilFormat(i);
	}
	// Create other Heaps

}

void DescriptorManager::CreateDepthStencilFormat(int i)
{
	RECT rc;
	int width = 0;
	int height = 0;
	if (GetClientRect(mp_graphicsDevice->GetWindow(), &rc))
	{
		width = rc.right - rc.left;   // largeur de la zone client
		height = rc.bottom - rc.top;    // hauteur de la zone client
	}

	// 1) Définir l’état initial, dimension, format
	D3D12_RESOURCE_DESC depthDesc = {};
	depthDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // Changer pour textures avancees ?
	depthDesc.Alignment = 0;
	depthDesc.Width = width;   // largeur de la fenetre
	depthDesc.Height = height; // hauteur de la fenetre
	depthDesc.DepthOrArraySize = 1;
	depthDesc.MipLevels = 1;
	depthDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// 2) Clear value (1.0 = far plane)
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = DXGI_FORMAT_D32_FLOAT;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	// 3) Créer la ressource
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
	ID3D12Resource* depthResource = nullptr;
	mp_graphicsDevice->GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&depthDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // état initial
		&clearValue,
		IID_PPV_ARGS(&depthResource)
	);

	// 4) Créer la DSV dans le heap DSV
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvViewDesc = {};
	dsvViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvViewDesc.Flags = D3D12_DSV_FLAG_NONE;

	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(
		m_dsvHeap->GetCPUDescriptorHandleForHeapStart(),
		i,
		m_dsvDescriptorSize
	);
	mp_graphicsDevice->GetDevice()->CreateDepthStencilView(depthResource, &dsvViewDesc, dsvHandle);

	// Stocke depthResource quelque part pour le release, et le DSV handle
	m_depthResources[i] = depthResource;
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
	mp_graphicsDevice = nullptr;
	delete mp_graphicsDevice;
}
