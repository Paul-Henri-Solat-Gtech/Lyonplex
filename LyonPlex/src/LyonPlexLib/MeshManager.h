#pragma once

#include "GraphicsDevice.h"

struct MeshParam
{
	DirectX::XMFLOAT3 Position;
	//DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT4 Color;
	//DirectX::XMFLOAT2 TexCoord;
};
struct GeometryMesh
{
	ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	ComPtr<ID3D12Resource> m_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

	// Nombre d'indices pour le rendu
	UINT m_meshIndex;
};
struct Mesh
{
	// typeOf Geometry
	GeometryMesh m_geometryMesh;

	// Buffer de constantes (matrices, couleurs, etc.)
	ComPtr<ID3D12Resource> m_constantBuffer;

	float width, height;

	void* m_mappedData = nullptr;
};

class MeshManager
{
public:
	void Init(GraphicsDevice* graphicsDevice);

	D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return m_vertexBufferView; };
	D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return m_indexBufferView; };

private:
	void AddNewMesh(MeshParam vertices[], size_t vertexCount, uint16_t indices[], size_t indexCount);

	void InitializeMesh_Triangle();
	void InitializeMesh_Square();
	void InitializeMesh_Cube();

	GraphicsDevice* mp_graphicsDevice;

	// Contient tous les mesh en un tableau (not built yet)
	void* m_pVertexData = nullptr;
	void* m_pIndexData = nullptr;

	//
	//ComPtr<ID3D12Resource> m_vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	//ComPtr<ID3D12Resource> m_indexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

	// Nombre d'indices pour le rendu
	UINT m_meshIndex;
};

