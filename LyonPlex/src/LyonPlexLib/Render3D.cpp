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

	//Draw vertices (mesh)
	mp_commandManager->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mp_commandManager->GetCommandList()->IASetVertexBuffers(0, 1, &m_meshManager.GetVertexBufferView());
	mp_commandManager->GetCommandList()->IASetIndexBuffer(&m_meshManager.GetIndexBufferView());
	mp_commandManager->GetCommandList()->DrawIndexedInstanced(3, 1, 0, 0, 0);
}

void Render3D::CreatePipeline()
{
	m_graphicsPipeline.CreatePipeline();
}