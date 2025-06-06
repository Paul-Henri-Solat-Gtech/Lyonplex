﻿#include "pch.h"
#include "Render3D.h"

bool Render3D::Init(HWND windowHandle, ECSManager* ECS, GraphicsDevice* graphicsDevice, DescriptorManager* descriptorManager, CommandManager* commandManager)
{
	m_ECS = ECS;

	mp_graphicsDevice = graphicsDevice;
	mp_descriptorManager = descriptorManager;
	mp_commandManager = commandManager;
	m_windowWP = windowHandle;


	m_graphicsPipeline.Init(mp_graphicsDevice, mp_descriptorManager, mp_commandManager);
	m_meshManager.Init(mp_graphicsDevice);

	InitConstantBuffer();
	UpdateCbParams();
	//Entity eTriangle = m_ECS->CreateEntity();
	//m_ECS->AddComponent<MeshComponent>(eTriangle, new MeshComponent(1, 0));
	//
	//Entity eSquare = m_ECS->CreateEntity();
	//m_ECS->AddComponent<MeshComponent>(eSquare, new MeshComponent(0, 0));

	//Entity eCube = m_ECS->CreateEntity();
	//m_ECS->AddComponent<MeshComponent>(eCube, new MeshComponent(2, 0));

	return true;
}

void Render3D::Resize(int w, int h)
{
	// Pas besoin ?
}

void Render3D::RecordCommands()
{
	// Definir le viewport et le scissor  (peuvent servir a "fenetrer" de l'affichage, par exemple pour minimap) : PEUT ETRE GENERAL OU VARIABLE
	// Le viewport represente la zone de la fenetre dans laquelle on va dessiner
	D3D12_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 800;  // window width
	viewport.Height = 600;  // window height
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// Le scissor defini un rectangle de pixels a dessiner dans la zone de dessin (viewport). Tous les pixels en dehors de cette zone ne sont pas dessines.
	D3D12_RECT scissorRect = { 0, 0, 800, 600 };

	// On definie la pipeline et la rootSignature
	mp_commandManager->GetCommandList()->SetGraphicsRootSignature(m_graphicsPipeline.GetRootSignature().Get());
	mp_commandManager->GetCommandList()->SetPipelineState(m_graphicsPipeline.GetPipelineState().Get());

	// Actualisation du rectangle dans lequel on dessine, dans la fenetre
	mp_commandManager->GetCommandList()->RSSetViewports(1, &viewport);
	mp_commandManager->GetCommandList()->RSSetScissorRects(1, &scissorRect);

	UINT frameIndex = mp_graphicsDevice->GetFrameIndex();
	// calcule a la volee le handle CPU vers le i-eme RTV
	auto rtvHandle = mp_descriptorManager->GetRtvHeap()->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += UINT64(frameIndex) * mp_descriptorManager->GetRtvDescriptorSize();

	// On definie le RTV sur lequel on va dessiner (pour nous les 3 RTV sont les memes je crois)
	mp_commandManager->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	const float clearColor[] = { 0.0f, 0.7f, 0.7f, 1.0f };
	mp_commandManager->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	// Ajouter ClearDepthStencilView() quand on l'aura ajoute a la pipeline


	mp_commandManager->GetCommandList()->SetGraphicsRootConstantBufferView(
		/*rootParameterIndex=*/ 0,
		m_ECS->m_systemMgr.GetCameraSystem().GetCBbuffer()->GetGPUVirtualAddress()
	);

	//Draw vertices (mesh)
	mp_commandManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mp_commandManager->GetCommandList()->IASetVertexBuffers(0, 1, &m_meshManager.GetGlobalVBView());
	mp_commandManager->GetCommandList()->IASetIndexBuffer(&m_meshManager.GetGlobalIBView());

	ComponentMask renderMask = 1ULL << MeshComponent::StaticTypeID;
	// Boucle sur toutes les entity a dessiner (componentMask MeshComponent)
	m_ECS->ForEach(renderMask, [&](Entity ent)
		{
			//const Material& mat = materialLib.Get(m->materialID);

			UpdateAndBindCB(ent); 

			MeshComponent* m = m_ECS->GetComponent< MeshComponent>(ent);
			const MeshData& data = m_meshManager.GetMeshLib().Get(m->meshID);
			mp_commandManager->GetCommandList()->DrawIndexedInstanced(
				data.iSize,      // nombre d’indices
				1,
				data.iOffset,    // offset dans le buffer d’indices
				0,    // BaseVertexLocation toujours = 0 ?
				0
			);
		});
}

void Render3D::CreatePipeline()
{
	m_graphicsPipeline.CreatePipeline();
}

void Render3D::Release()
{
	mp_graphicsDevice = nullptr;
	delete mp_graphicsDevice;

	mp_descriptorManager = nullptr;
	delete mp_descriptorManager;

	mp_commandManager = nullptr;
	delete mp_commandManager;
}


//-----------------------------------------------------------------------------//
// Gestion du Constant Buffer (transform)
//-----------------------------------------------------------------------------//

bool Render3D::InitConstantBuffer()
{
	// Definir les proprietes du heap « upload »
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);

	// Decrire un buffer de m_cbSize octets
	CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(m_cbSize);

	// Creer le buffer upload
	HRESULT hr = mp_graphicsDevice->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_cbTransformUpload));
	if (FAILED(hr))
	{
		return false;
	}

	// 5) Mapper une seule fois pour recuperer l’adresse CPU
	CD3DX12_RANGE readRange(0, 0); // on ne lit jamais CPU→GPU
	hr = m_cbTransformUpload->Map(0, &readRange, reinterpret_cast<void**>(&m_mappedCBData));
	if (FAILED(hr))
	{
		return false;
	}
}

void Render3D::UpdateCbParams()
{
	UINT m_entityCount = m_ECS->GetEntityCount();
	UINT m_frameCount = mp_graphicsDevice->GetFrameCount();
	UINT totalSize = m_cbSize * m_entityCount * m_frameCount;
}

void Render3D::UpdateAndBindCB(Entity ent)
{
	// 1) Recuperer TransformComponent de l’entite
	auto* tc = m_ECS->GetComponent<TransformComponent>(ent);
	if (!tc)
		return;

	// 2) Calculer la matrice monde (XMMATRIX) depuis tc->position/rotation/scale
	//    (votre TransformSystem aura deja mis a jour worldMatrices)
	XMMATRIX world = m_ECS->m_systemMgr.GetTransformSystem().worldMatrices[ent.id];

	// 3) Construire le struct ConstantBuffData
	ConstantBuffData cbData;
	XMStoreFloat4x4(&cbData.World, XMMatrixTranspose(world));


	UpdateCbParams();
	UINT entityOffset = ent.id * m_cbSize;
	UINT frameOffset = mp_graphicsDevice->GetFrameIndex() * m_entityCount * m_cbSize;
	UINT finalOffset = frameOffset + entityOffset;



	//// 4) Copier les donnees dans le buffer upload mappe
	//memcpy(m_mappedCBData, &cbData, sizeof(ConstantBuffData));
	memcpy((BYTE*)m_mappedCBData + finalOffset, &cbData, sizeof(ConstantBuffData));

	// 5) Binder le constant buffer au root slot 0 (register b0)
	//    Remarquez : on donne l’adresse GPU virtuelle du buffer entier
	mp_commandManager->GetCommandList()->SetGraphicsRootConstantBufferView(/*rootParameterIndex=*/ 1, m_cbTransformUpload->GetGPUVirtualAddress() + finalOffset);
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = m_cbTransformUpload->GetGPUVirtualAddress();
}
