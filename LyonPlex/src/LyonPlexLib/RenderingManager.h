#pragma once

#include "GraphicsDevice.h"
#include "DescriptorManager.h"
#include "CommandManager.h"
#include "Render3D.h"

class RenderingManager
{
public:
	bool Init();
	
	void SetWindowHandle(HWND windowHandle) { m_windowWP = windowHandle; }

	void RecordCommands();	// Enregistre dans la CommandList les instructions de rendu pour chaque frame (par exemple un draw)
	void ExecuteCommands(); // Envoie la CommandList enregistrée au GPU via la CommandQueue
	void Present();			// Demande à la swap chain de présenter le back buffer courant à l’écran
	void SignalAndWait();	// Synchronise le CPU et la GPU en signalant la fence et en attendant sa complétion

private:
	
	HWND m_windowWP;		// The created base window (pour le swapchain)

	// Instance de class de directX
	GraphicsDevice m_graphicsDevice;
	DescriptorManager m_descriptorManager;
	CommandManager m_commandManager;
	Render3D m_render3D;
};

