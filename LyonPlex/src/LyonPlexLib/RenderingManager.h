#pragma once

#include "GraphicsDevice.h"
#include "DescriptorManager.h"
#include "CommandManager.h"

class RenderingManager
{
public:
	bool Init();
	
	void SetWindowHandle(HWND windowHandle) { m_windowWP = windowHandle; }

	void CreatePipeline();	// Managing chain for 3D and 2D elements (data not draw)
	void RecordCommands();	// Enregistre dans la CommandList les instructions de rendu pour chaque frame (par exemple un draw)
	void ExecuteCommands(); // Envoie la CommandList enregistrée au GPU via la CommandQueue
	void Present();			// Demande à la swap chain de présenter le back buffer courant à l’écran
	void SignalAndWait();	// Synchronise le CPU et la GPU en signalant la fence et en attendant sa complétion

private:
	
	HWND m_windowWP;		// The created base window (pour le swapchain)

	// COM objects
	ComPtr<ID3D12Resource> m_renderTargets[2];			// Les textures (back buffers) où le rendu est effectué (taille FrameCount)

	// Pipeline graphique
	ComPtr<ID3D12RootSignature> m_rootSignature;		// Décrit les ressources accessibles par les shaders
	ComPtr<ID3D12PipelineState> m_pipelineState;		// Contient l'état complet du pipeline graphique
	ComPtr<ID3D12Resource> m_vertexBuffer;				// Buffer contenant les sommets à dessiner
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;		// Vue permettant d’accéder au vertex buffer dans le pipeline

	// Instance de class de directX
	GraphicsDevice m_graphicsDevice;
	DescriptorManager m_descriptorManager;
	CommandManager m_commandManager;
};

