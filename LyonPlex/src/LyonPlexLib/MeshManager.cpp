#include "pch.h"
#include "MeshManager.h"

void MeshManager::Init(GraphicsDevice* graphicsDevice)
{
	mp_graphicsDevice = graphicsDevice;

	// Initialisation de tous les mesh
	//InitializeMesh_Cube();
	InitializeMesh_Triangle();
	InitializeMesh_Square();
	InitializeMesh_Cube();
	// ...


	// Build & upload meshs
	BuildAndUploadGlobalBuffers();

	//InitConstantBuffer();

}


MeshData MeshManager::CreateMesh_Triangle()
{
	MeshData m;
	m.vertices =
	{
		{{ 0.0f,  0.25f, 0.0f},{1,1,0,1}},
		{{ 0.25f,-0.25f, 0.0f},{0,0,0,1}},
		{{-0.25f,-0.25f, 0.0f},{0,0,0,1}},
	};

	// Definition des indices pour dessiner 2 triangles
	m.indices = { 0, 1, 2 };

	m.vSize = static_cast<uint32_t>(m.vertices.size());
	m.iSize = static_cast<uint32_t>(m.indices.size());


	return m;
}
MeshData MeshManager::CreateMesh_Square()
{
	MeshData m;
	m.vertices =
	{
		// coin haut-gauche
		{{ -0.25f, 0.25f, 0.0f},{5,1,0,1}},
		// coin bas-gauche
		{{ -0.25f, -0.25f, 0.0f},{0,0,0,1}},
		// coin bas-droit
		{{ 0.25f, -0.25f, 0.0f},{0,0,0,1}},
		// coin haut-droit
		{{ 0.25f, 0.25f, 0.0f},{0,0,0,1}},
	};

	// Definition des indices pour dessiner 2 triangles
	m.indices =
	{
		0, 2, 1,  // triangle bas-gauche → bas-droit
		0, 3, 2   // triangle bas-gauche → haut-droit
	};

	m.vSize = static_cast<uint32_t>(m.vertices.size());
	m.iSize = static_cast<uint32_t>(m.indices.size());
	return m;
}
MeshData MeshManager::CreateMesh_Cube()
{
	MeshData m;
	m.vertices =
	{
		// Face avant
		{{ -0.25f,  0.25f, -0.25f },{1,0,0,1}}, // 0
		{{ -0.25f, -0.25f, -0.25f },{0,1,0,1}}, // 1
		{{  0.25f, -0.25f, -0.25f },{0,0,1,1}}, // 2
		{{  0.25f,  0.25f, -0.25f },{1,1,0,1}}, // 3

		// Face arriere
		{{ -0.25f,  0.25f,  0.25f },{1,0,1,1}}, // 4
		{{ -0.25f, -0.25f,  0.25f },{0,1,1,1}}, // 5
		{{  0.25f, -0.25f,  0.25f },{1,1,1,1}}, // 6
		{{  0.25f,  0.25f,  0.25f },{0,0,0,1}}, // 7
	};

	// Definition des indices pour dessiner 2 triangles
	m.indices =
	{
		// Face avant
		0, 1, 2,  0, 2, 3,
		// Face arriere
		4, 6, 5,  4, 7, 6,
		// Face gauche
		4, 5, 1,  4, 1, 0,
		// Face droite
		3, 2, 6,  3, 6, 7,
		// Face haut
		4, 0, 3,  4, 3, 7,
		// Face bas
		1, 5, 6,  1, 6, 2
	};

	m.vSize = static_cast<uint32_t>(m.vertices.size());
	m.iSize = static_cast<uint32_t>(m.indices.size());
	return m;
}

HRESULT MeshManager::BuildAndUploadGlobalBuffers()
{
	std::vector<VertexParam> globalVerts;
	std::vector<uint16_t>  globalIdxs;
	size_t vertexCursor = 0;
	size_t indexCursor = 0;

	// Parcours chaque MeshData enregistre (du plus petit ID au plus grand)
	for (uint32_t id = 0; id < m_meshLibrary.Count(); ++id) 
	{
		MeshData& md = m_meshLibrary.Get(id);

		// 1) Calculer et stocker les offsets
		md.vOffset = static_cast<uint32_t>(vertexCursor);
		md.vSize = static_cast<uint32_t>(md.vertices.size());
		md.iOffset = static_cast<uint32_t>(indexCursor);
		md.iSize = static_cast<uint32_t>(md.indices.size());

		// 2) Ajouter les sommets de ce maillage a la suite
		globalVerts.insert(globalVerts.end(), md.vertices.begin(), md.vertices.end());

		// 3) Ajouter les indices, corriges par le decalage vOffset
		for (auto idx : md.indices) 
		{
			globalIdxs.push_back(static_cast<uint16_t>(idx + md.vOffset));
		}

		vertexCursor += md.vSize;
		indexCursor += md.iSize;
	}


	UINT vByteSize = UINT(globalVerts.size() * sizeof(VertexParam));
	UINT iByteSize = UINT(globalIdxs.size() * sizeof(uint16_t));

	D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	// Vertex Buffer
	{
		auto vbDesc = CD3DX12_RESOURCE_DESC::Buffer(vByteSize);
		mp_graphicsDevice->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &vbDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_globalVertexBuffer));

		void* pData = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		m_globalVertexBuffer->Map(0, &readRange, &pData);
		memcpy(pData, globalVerts.data(), vByteSize);
		m_globalVertexBuffer->Unmap(0, nullptr);

		m_globalVBView.BufferLocation = m_globalVertexBuffer->GetGPUVirtualAddress();
		m_globalVBView.StrideInBytes = sizeof(VertexParam);
		m_globalVBView.SizeInBytes = vByteSize;
	}

	// Index Buffer
	{
		auto ibDesc = CD3DX12_RESOURCE_DESC::Buffer(iByteSize);
		mp_graphicsDevice->GetDevice()->CreateCommittedResource( &heapProps, D3D12_HEAP_FLAG_NONE, &ibDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_globalIndexBuffer));

		void* pData = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		m_globalIndexBuffer->Map(0, &readRange, &pData);
		memcpy(pData, globalIdxs.data(), iByteSize);
		m_globalIndexBuffer->Unmap(0, nullptr);

		m_globalIBView.BufferLocation = m_globalIndexBuffer->GetGPUVirtualAddress();
		m_globalIBView.Format = DXGI_FORMAT_R16_UINT;
		m_globalIBView.SizeInBytes = iByteSize;
	}

	return E_NOTIMPL;
}

void MeshManager::InitializeBasicShapes()
{
	InitializeMesh_Triangle();
}

void MeshManager::InitializeMesh_Triangle()
{
	MeshData tri = CreateMesh_Triangle();
	auto triID = m_meshLibrary.Add(tri);
}

void MeshManager::InitializeMesh_Square()
{
	MeshData square = CreateMesh_Square();
	auto squareID = m_meshLibrary.Add(square);
}

void MeshManager::InitializeMesh_Cube()
{
	MeshData cube = CreateMesh_Cube();
	auto cubeID = m_meshLibrary.Add(cube);

	//Mesh* newMesh = new Mesh;

	//VertexParam cube[] =
	//{
	//	// Face avant
	//	{{ -0.25f,  0.25f, -0.25f },{1,0,0,1}}, // 0
	//	{{ -0.25f, -0.25f, -0.25f },{0,1,0,1}}, // 1
	//	{{  0.25f, -0.25f, -0.25f },{0,0,1,1}}, // 2
	//	{{  0.25f,  0.25f, -0.25f },{1,1,0,1}}, // 3

	//	// Face arriere
	//	{{ -0.25f,  0.25f,  0.25f },{1,0,1,1}}, // 4
	//	{{ -0.25f, -0.25f,  0.25f },{0,1,1,1}}, // 5
	//	{{  0.25f, -0.25f,  0.25f },{1,1,1,1}}, // 6
	//	{{  0.25f,  0.25f,  0.25f },{0,0,0,1}}, // 7
	//};

	//// Definition des indices pour dessiner 2 triangles
	//uint16_t indices[] =
	//{
	//	// Face avant
	//	0, 1, 2,  0, 2, 3,
	//	// Face arriere
	//	4, 6, 5,  4, 7, 6,
	//	// Face gauche
	//	4, 5, 1,  4, 1, 0,
	//	// Face droite
	//	3, 2, 6,  3, 6, 7,
	//	// Face haut
	//	4, 0, 3,  4, 3, 7,
	//	// Face bas
	//	1, 5, 6,  1, 6, 2
	//};

	//AddNewMeshToList(newMesh, cube, indices, _countof(cube), _countof(indices));
}

