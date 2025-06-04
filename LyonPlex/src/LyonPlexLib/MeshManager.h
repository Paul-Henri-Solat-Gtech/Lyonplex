#pragma once

#include "GraphicsDevice.h"
#include "ResourceManager.h"

struct VertexParam
{
	XMFLOAT3 Position;		// Position des points
	XMFLOAT4 Color;			// Couleur des points
	//XMFLOAT2 TexCoord;	// Coordonees du point sur la texture ?
	//XMFLOAT3 Normal;		// Normale ・la face visible du mesh (vecteur de direction)
};


struct MeshData
{
	std::vector<VertexParam> vertices;   // ici, on stocke directement les VertexParam (pas des pointeurs pcq struct legere)
	std::vector<uint16_t> indices;
	UINT vOffset = 0;   // decalage dans le buffer global
	UINT vSize = 0;   // nombre de sommets
	UINT iOffset = 0;   // decalage dans le buffer global
	UINT iSize = 0;   // nombre d’indices

	bool LoadFromFile(const std::string& path)
	{
		// ici tu lis le fichier (obj, fbx, gltf, etc.),
		// tu fills `vertices` et `indices`,
		// et tu retournes true/false suivant le succes.
	}
	void Unload()
	{
		// si tu veux desallouer ou liberer toute ressource GPU associee
		vertices.clear();
		indices.clear();
	}
};

class MeshManager
{
public:
	void Init(GraphicsDevice* graphicsDevice);

	D3D12_VERTEX_BUFFER_VIEW& GetGlobalVBView() { return m_globalVBView; };
	D3D12_INDEX_BUFFER_VIEW& GetGlobalIBView() { return m_globalIBView; };

	ResourceManager<MeshData>& GetMeshLib() { return m_meshLibrary; }

private:

	MeshData CreateMesh_Triangle();
	MeshData CreateMesh_Square();
	MeshData CreateMesh_Cube();

	HRESULT BuildAndUploadGlobalBuffers();

	void InitializeBasicShapes();

	void InitializeMesh_Triangle();
	void InitializeMesh_Square();
	void InitializeMesh_Cube();


	GraphicsDevice* mp_graphicsDevice;


	// Variables de Vertex et Index globales
	ComPtr<ID3D12Resource> m_globalVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW m_globalVBView;

	ComPtr<ID3D12Resource> m_globalIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW m_globalIBView;

	// Buffers globaux (membres de la classe)
	std::vector<VertexParam> m_globalVertices;
	std::vector<uint16_t>  m_globalIndices;

	// Bibliotheque de meshs
	ResourceManager<MeshData> m_meshLibrary;

};

