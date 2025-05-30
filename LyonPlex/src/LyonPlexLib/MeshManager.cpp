#include "pch.h"
#include "MeshManager.h"

void MeshManager::Init(GraphicsDevice* graphicsDevice)
{
	mp_graphicsDevice = graphicsDevice;

	// Initialisation de tous les mesh
	InitializeMesh_Square();
	InitializeMesh_Cube();
	InitializeMesh_Triangle();
	// ...


	// Build & upload meshs
	BuildAndUploadGlobalBuffers();
}

HRESULT MeshManager::BuildAndUploadGlobalBuffers()
{
	m_globalVertices.clear();
	m_globalIndices.clear();

	// (Optionnel) r駸erver d'avance pour optimiser
	UINT totalVerts = 0, totalInds = 0;
	for (auto& m : m_meshList) {
		totalVerts += static_cast<UINT>(m->m_meshParamTabl.size());
		totalInds += static_cast<UINT>(m->m_indexTabl.size());
	}
	m_globalVertices.reserve(totalVerts);
	m_globalIndices.reserve(totalInds);

	// Calcul taille totale et offsets
	UINT vCursor = 0, iCursor = 0;
	for (auto& m : m_meshList)
	{
		// mettre ・jour offsets/sizes
		m->m_vOffset = vCursor;
		m->m_vSize = static_cast<UINT>(m->m_meshParamTabl.size());
		m->m_iOffset = iCursor;
		m->m_iSize = static_cast<UINT>(m->m_indexTabl.size());

		// copier les MeshParam
		for (MeshParam* p : m->m_meshParamTabl)
			m_globalVertices.push_back(*p);

		// copier les indices avec correction d'offset
		for (uint16_t idx : m->m_indexTabl)
			//m_globalIndices.push_back(static_cast<uint16_t>(idx + vCursor)); //ne vas pas avec la boucle de render3D (!)
			m_globalIndices.push_back(idx);

		vCursor += m->m_vSize;
		iCursor += m->m_iSize;
	}

	// 2) Création et upload buffers GPU
	// Ta logique D3D12 r駱騁馥 pour vertex et index, sans passer par arguments
	UINT vByteSize = UINT(m_globalVertices.size() * sizeof(MeshParam));
	UINT iByteSize = UINT(m_globalIndices.size() * sizeof(uint16_t));
	D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

	// Vertex buffer
	{
		auto vbDesc = CD3DX12_RESOURCE_DESC::Buffer(vByteSize);
		HRESULT hr = mp_graphicsDevice->GetDevice()->CreateCommittedResource( &heapProps, D3D12_HEAP_FLAG_NONE, &vbDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_globalVertexBuffer));
		if (FAILED(hr)) return hr;

		void* pData = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		hr = m_globalVertexBuffer->Map(0, &readRange, &pData);
		if (FAILED(hr)) return hr;
		memcpy(pData, m_globalVertices.data(), vByteSize);
		m_globalVertexBuffer->Unmap(0, nullptr);

		m_globalVBView.BufferLocation = m_globalVertexBuffer->GetGPUVirtualAddress();
		m_globalVBView.StrideInBytes = sizeof(MeshParam);
		m_globalVBView.SizeInBytes = vByteSize;
	}

	// Index buffer
	{
		auto ibDesc = CD3DX12_RESOURCE_DESC::Buffer(iByteSize);
		HRESULT hr = mp_graphicsDevice->GetDevice()->CreateCommittedResource( &heapProps, D3D12_HEAP_FLAG_NONE, &ibDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_globalIndexBuffer));
		if (FAILED(hr)) return hr;

		void* pData = nullptr;
		CD3DX12_RANGE readRange(0, 0);
		hr = m_globalIndexBuffer->Map(0, &readRange, &pData);
		if (FAILED(hr)) return hr;
		memcpy(pData, m_globalIndices.data(), iByteSize);
		m_globalIndexBuffer->Unmap(0, nullptr);

		m_globalIBView.BufferLocation = m_globalIndexBuffer->GetGPUVirtualAddress();
		m_globalIBView.Format = DXGI_FORMAT_R16_UINT;
		m_globalIBView.SizeInBytes = iByteSize;
	}

	return S_OK;
}


void MeshManager::InitializeMesh_Triangle()
{
	Mesh* newMesh = new Mesh;

	MeshParam triangle[] =
	{
		{{ 0.0f,  0.25f, 0.0f},{1,1,0,1}},
		{{ 0.25f,-0.25f, 0.0f},{0,0,0,1}},
		{{-0.25f,-0.25f, 0.0f},{0,0,0,1}},
	};

	{
		int i;
		for (i = 0; i < _countof(triangle); i++)
		{
			// 1) allouer un nouvel objet
			MeshParam* nMesh = new MeshParam;
			// 2) copier le contenu de triangle[i] dans votre nMesh
			*nMesh = triangle[i];
			// 3) pousser la copie dans le vecteur
			newMesh->m_meshParamTabl.push_back(nMesh);
		}
		newMesh->m_vSize = i;
	}

	// Definition des indices pour dessiner 2 triangles
	uint16_t indices[] = { 0, 1, 2 };

	{
		int i;
		for (i = 0; i < _countof(triangle); i++)
		{
			newMesh->m_indexTabl.push_back(indices[i]);
		}
		newMesh->m_iSize = i;
	}

	m_meshList.push_back(newMesh);
}

void MeshManager::InitializeMesh_Square()
{
	Mesh* newMesh = new Mesh;

	MeshParam square[] =
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

	{
		int i;
		for (i = 0; i < _countof(square); i++) 
		{
			// 1) allouer un nouvel objet
			MeshParam* nMesh = new MeshParam;
			// 2) copier le contenu de triangle[i] dans votre nMesh
			*nMesh = square[i];
			// 3) pousser la copie dans le vecteur
			newMesh->m_meshParamTabl.push_back(nMesh);
		}
		newMesh->m_vSize = i;
	}

	// Definition des indices pour dessiner 2 triangles
	uint16_t indices[] = 
	{ 
		0, 2, 1,  // triangle bas-gauche → bas-droit
		0, 3, 2   // triangle bas-gauche → haut-droit
	};

	{
		int i;
		for (i = 0; i < _countof(indices); i++)
		{
			newMesh->m_indexTabl.push_back(indices[i]);
		}
		newMesh->m_iSize = i;
	}

	m_meshList.push_back(newMesh);
}

void MeshManager::InitializeMesh_Cube()
{
	Mesh* newMesh = new Mesh;

	MeshParam cube[] =
	{
		// Face avant
		{{ -0.25f,  0.25f, -0.25f },{1,0,0,1}}, // 0
		{{ -0.25f, -0.25f, -0.25f },{0,1,0,1}}, // 1
		{{  0.25f, -0.25f, -0.25f },{0,0,1,1}}, // 2
		{{  0.25f,  0.25f, -0.25f },{1,1,0,1}}, // 3

		// Face arrière
		{{ -0.25f,  0.25f,  0.25f },{1,0,1,1}}, // 4
		{{ -0.25f, -0.25f,  0.25f },{0,1,1,1}}, // 5
		{{  0.25f, -0.25f,  0.25f },{1,1,1,1}}, // 6
		{{  0.25f,  0.25f,  0.25f },{0,0,0,1}}, // 7
	};

	{
		int i;
		for (i = 0; i < _countof(cube); i++)
		{
			// 1) allouer un nouvel objet
			MeshParam* nMesh = new MeshParam;
			// 2) copier le contenu de triangle[i] dans votre nMesh
			*nMesh = cube[i];
			// 3) pousser la copie dans le vecteur
			newMesh->m_meshParamTabl.push_back(nMesh);
		}
		newMesh->m_vSize = i;
	}

	// Definition des indices pour dessiner 2 triangles
	uint16_t indices[] =
	{
		// Face avant
		0, 1, 2,  0, 2, 3,
		// Face arrière
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

	{
		int i;
		for (i = 0; i < _countof(indices); i++)
		{
			newMesh->m_indexTabl.push_back(indices[i]);
		}
		newMesh->m_iSize = i;
	}

	m_meshList.push_back(newMesh);
}
