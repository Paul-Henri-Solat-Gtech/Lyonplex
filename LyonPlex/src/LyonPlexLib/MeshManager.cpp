#include "pch.h"
#include "MeshManager.h"

void MeshManager::Init(GraphicsDevice* graphicsDevice)
{
	mp_graphicsDevice = graphicsDevice;

	InitializeMesh_Triangle();
}

void MeshManager::AddNewMesh(MeshParam vertices[], size_t vertexCount, uint16_t indices[], size_t indexCount)
{
	UINT vSize = static_cast<UINT>(vertexCount * sizeof(MeshParam));
	UINT iSize = static_cast<UINT>(indexCount * sizeof(uint16_t));
	//const UINT vSize = sizeof(vertices);
	//const UINT iSize = sizeof(indices);

	// VERTEX
	ComPtr<ID3D12Resource> vertexBuffer;
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	// INDEX
	ComPtr<ID3D12Resource> indexBuffer;
	//D3D12_INDEX_BUFFER_VIEW indexBufferView;

	// On utilise un heap UPLOAD pour permettre au CPU d'ecrire dans la memoire
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC vbDesc = CD3DX12_RESOURCE_DESC::Buffer(vSize);
	CD3DX12_RESOURCE_DESC ibDesc = CD3DX12_RESOURCE_DESC::Buffer(iSize);

	// 1)Creer le vertex buffer
	HRESULT hr = mp_graphicsDevice->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &vbDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer));
	if (FAILED(hr)) { /* Gerer l'erreur */ }

	// 2)Creer l'index buffer
	hr = mp_graphicsDevice->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &ibDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&indexBuffer));
	if (FAILED(hr)) { /* Gerer l'erreur */ }

	// Copier les donnees dans le vertex buffer
	//void* pVertexData = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	hr = vertexBuffer->Map(0, &readRange, &m_pVertexData);
	memcpy(m_pVertexData, vertices, vSize);
	vertexBuffer->Unmap(0, nullptr);

	// Copier les donnees dans l'index buffer
	//void* pIndexData = nullptr;
	hr = indexBuffer->Map(0, &readRange, &m_pIndexData);
	memcpy(m_pIndexData, indices, iSize);
	indexBuffer->Unmap(0, nullptr);

	// 3)Creer les vues buffer
	m_vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = sizeof(MeshParam);
	m_vertexBufferView.SizeInBytes = vSize;

	m_indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R16_UINT;
	m_indexBufferView.SizeInBytes = iSize;
}

void MeshManager::InitializeMesh_Triangle()
{
	MeshParam triangle[] =
	{
		{{ 0.0f,  0.25f, 0.0f},{1,0,0,1}},
		{{ 0.25f,-0.25f, 0.0f},{0,1,0,1}},
		{{-0.25f,-0.25f, 0.0f},{0,0,1,1}},
	};

	// Definition des indices pour dessiner 2 triangles
	uint16_t indices[] = { 0, 1, 2 };
	
	AddNewMesh(triangle, _countof(triangle), indices, _countof(indices));

}
