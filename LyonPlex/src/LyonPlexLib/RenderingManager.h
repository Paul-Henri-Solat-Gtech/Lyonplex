#pragma once

// Hierarchie superieure ou egale
//#include "ECSManager.h"
//class ECSManager;

// Hierarchie inferieure
#include "GraphicsDevice.h"
#include "DescriptorManager.h"
#include "CommandManager.h"
#include "CameraManager.h"
#include "Render3D.h"

class RenderingManager
{
public:
	bool Init(ECSManager* ECS);
	
	void SetWindowHandle(HWND windowHandle) { m_windowWP = windowHandle; }

	void RecordCommands();	// Enregistre dans la CommandList les instructions de rendu pour chaque frame (par exemple un draw)
	
	void SetBarrierToRenderTarget(CD3DX12_RESOURCE_BARRIER& barrier);
	void SetBarrierToPresent(CD3DX12_RESOURCE_BARRIER& barrier);
	
	void ExecuteCommands(); // Envoie la CommandList enregistree au GPU via la CommandQueue
	void Present();			// Demande a la swap chain de presenter le back buffer courant a l’ecran
	void SynchroGPUCPU();	// Synchronise le CPU et la GPU en signalant la fence et en attendant sa completion


	DescriptorManager& GetDescriptorManager() { return m_descriptorManager; }

	GraphicsDevice* GetGraphicsDevice() { return &m_graphicsDevice; }
	CommandManager* GetCommandManager() { return &m_commandManager; }
	Render3D* GetRender3D() { return &m_render3D; }


	void Release();

	void Update();

private:
	
	HWND m_windowWP;		// The created base window (pour le swapchain)

	//ECS Manager
	ECSManager* mp_ECS;

	// Instance de class de directX
	GraphicsDevice m_graphicsDevice;
	DescriptorManager m_descriptorManager;
	CommandManager m_commandManager;
	Render3D m_render3D;
	//CameraManager m_cameraManager;
};

