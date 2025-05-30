#pragma once

#include "GraphicsDevice.h"

struct MeshParam
{
	DirectX::XMFLOAT3 Position;		// Position des points
	DirectX::XMFLOAT4 Color;		// Couleur des points
	//DirectX::XMFLOAT2 TexCoord;	// Coordonées du point sur la texture ?
	//DirectX::XMFLOAT3 Normal;		// Normale à la face visible du mesh (vecteur de direction)
};

struct Mesh
{
	std::vector<MeshParam*> m_meshParamTabl;
	UINT m_vSize = 0;
	UINT m_vOffset = 0;

	std::vector<uint16_t> m_indexTabl;
	UINT m_iSize = 0;
	UINT m_iOffset = 0;
};

class MeshManager
{
public:
	void Init(GraphicsDevice* graphicsDevice);

	D3D12_VERTEX_BUFFER_VIEW& GetGlobalVBView() { return m_globalVBView; };
	D3D12_INDEX_BUFFER_VIEW& GetGlobalIBView() { return m_globalIBView; };

private:

	void AddNewMeshToList(Mesh* newMesh, MeshParam triangle[], uint16_t indices[], int triangleSize, int indiceSize);

	HRESULT BuildAndUploadGlobalBuffers();

	void InitializeMesh_Triangle();
	void InitializeMesh_Test();
	void InitializeMesh_Square();
	void InitializeMesh_Cube();

	GraphicsDevice* mp_graphicsDevice;

	//std::vector<MeshParam*> m_meshList;
	std::vector<Mesh*> m_meshList;

	// Variables de Vertex et Index globales
	ComPtr<ID3D12Resource> m_globalVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_globalVBView;

	ComPtr<ID3D12Resource> m_globalIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_globalIBView;

	// Buffers globaux (membres de la classe)
	std::vector<MeshParam> m_globalVertices;
	std::vector<uint16_t>  m_globalIndices;
};

