#include "pch.h"
#include "Render3D.h"

bool Render3D::Init(HWND windowHandle, GraphicsDevice* graphicsDevice, DescriptorManager* descriptorManager, CommandManager* commandManager)
{
	mp_graphicsDevice = graphicsDevice;
	mp_descriptorManager = descriptorManager;
	mp_commandManager = commandManager;
	m_windowWP = windowHandle;

	m_graphicsPipeline.Init(mp_graphicsDevice, mp_descriptorManager, mp_commandManager);
	m_meshManager.Init(mp_graphicsDevice);

	//changer emplacement init camera
	mp_cameraManager = new CameraManager();
	mp_cameraManager->Init();
	mp_cameraManager->SetPosition({ 0.0f, 0.0f, -2.0f });
	mp_cameraManager->SetDirection({ 0.0f, 0.0f, 1.0f });
	mp_cameraManager->UpdateProjectionMatrix(90.0f, 800.f / 600.f, 0.1f, 1000.f);
	CreateCameraBuffer();

	return true;
}

void Render3D::Resize(int w, int h)
{

}

void Render3D::RecordCommands()
{

	// Définir le viewport et le scissor  (peuvent servir a "fenetrer" de l'affichage, par exemple pour minimap) : PEUT ETRE GENERAL OU VARIABLE
	// Le viewport représente la zone de la fenêtre dans laquelle on va dessiner
	D3D12_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = 800;  // window width
	viewport.Height = 600;  // window height
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// Le scissor défini un rectangle de pixels à dessiner dans la zone de dessin (viewport). Tous les pixels en dehors de cette zone ne sont pas dessinés.
	D3D12_RECT scissorRect = { 0, 0, 800, 600 };

	// On définie la pipeline et la rootSignature
	mp_commandManager->GetCommandList()->SetGraphicsRootSignature(m_graphicsPipeline.GetRootSignature().Get());
	mp_commandManager->GetCommandList()->SetPipelineState(m_graphicsPipeline.GetPipelineState().Get());

	// Actualisation du rectangle dans lequel on dessine, dans la fenetre
	mp_commandManager->GetCommandList()->RSSetViewports(1, &viewport);
	mp_commandManager->GetCommandList()->RSSetScissorRects(1, &scissorRect);

	UINT frameIndex = mp_graphicsDevice->GetFrameIndex();
	// calcule à la volée le handle CPU vers le i-ème RTV
	auto rtvHandle = mp_descriptorManager->GetRtvHeap()->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += UINT64(frameIndex) * mp_descriptorManager->GetRtvDescriptorSize();

	// On définie le RTV sur lequel on va dessiner (pour nous les 3 RTV sont les mêmes je crois)
	mp_commandManager->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

	const float clearColor[] = { 0.0f, 0.7f, 0.7f, 1.0f };
	mp_commandManager->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	// Ajouter ClearDepthStencilView() quand on l'aura ajouté a la pipeline

	//----camtest
	// 1) Met à jour la vue
	mp_cameraManager->UpdateViewMatrix();

	// 2) Récupère directement le CameraBuffer (déjà transposé)
	CameraBuffer cbData = mp_cameraManager->GetCameraBuffer();

	// 3) Upload en une fois dans le CBV
	UINT8* mapped = nullptr;
	CD3DX12_RANGE readRange(0, 0);
	m_cameraCB->Map(0, &readRange, reinterpret_cast<void**>(&mapped));
	memcpy(mapped, &cbData, sizeof(cbData));
	m_cameraCB->Unmap(0, nullptr);

	// 4) Bind au slot b0
	mp_commandManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, m_cameraCB->GetGPUVirtualAddress());
	//----camtest

	//Draw vertices (mesh)
	mp_commandManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mp_commandManager->GetCommandList()->IASetVertexBuffers(0, 1, &m_meshManager.GetGlobalVBView());
	mp_commandManager->GetCommandList()->IASetIndexBuffer(&m_meshManager.GetGlobalIBView());
	
	// Ligne à répéter dans une boucle d'itération sur les entités
	//mp_commandManager->GetCommandList()->DrawIndexedInstanced(3, 1, 0, 0, 0); // Arguments à changer : vSize   // InstanceCount   // iOffset   // vOffset    // StartInstanceLocation (0)

	for (auto& mesh : m_meshManager.GetMeshList())
	{
		mp_commandManager->GetCommandList()->DrawIndexedInstanced(
			mesh->m_iSize,      // nombre d’indices
			1,
			mesh->m_iOffset,    // offset dans le buffer d’indices
			mesh->m_vOffset,    // BaseVertexLocation = 0 !
			//0,
			0
		);
	}
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

	mp_cameraManager = nullptr;
	delete mp_cameraManager;
}

//cam temp
void Render3D::UpdateTemp()
{
	float deltaTime = 1.0f / 60.0f;
	float speed = 0.2;
	if(GetAsyncKeyState('Z') & 0x8000)
		mp_cameraManager->MoveForward(speed * deltaTime);
	if (GetAsyncKeyState('S') & 0x8000)
		mp_cameraManager->MoveForward(-speed * deltaTime);
	if (GetAsyncKeyState('D') & 0x8000)
		mp_cameraManager->MoveRight(speed * deltaTime);
	if (GetAsyncKeyState('Q') & 0x8000)
		mp_cameraManager->MoveRight(-speed * deltaTime);
	if (GetAsyncKeyState(VK_SPACE) & 0x8000)
		mp_cameraManager->MoveUp(speed * deltaTime); // À créer si pas déjà là
	if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
		mp_cameraManager->MoveUp(-speed * deltaTime); // descend

	mp_cameraManager->UpdateViewMatrix();

	XMFLOAT3 pos = mp_cameraManager->GetPosition();
	wchar_t title[128];
	swprintf(title, 128, L"MonJeuDX12 - Caméra [X: %.2f, Y: %.2f, Z: %.2f]", pos.x, pos.y, pos.z);

	::SetWindowText(m_windowWP, title); // windowHandle doit être un HWND valide

}

//cam temp
void Render3D::CreateCameraBuffer()
{
	UINT64 sizeCB = (sizeof(CameraBuffer) + 255) & ~255;
	CD3DX12_HEAP_PROPERTIES hp(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC rd = CD3DX12_RESOURCE_DESC::Buffer(sizeCB);

	mp_graphicsDevice->GetDevice()->CreateCommittedResource(
		&hp, D3D12_HEAP_FLAG_NONE, &rd,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&m_cameraCB));
}