#pragma once

#include "IRender.h"
#include "GraphicsPipeline.h"
#include "MeshManager.h"
#include "CameraManager.h"

class Render3D : public IRender
{
public:
	bool Init(HWND windowHandle, GraphicsDevice* graphicsDevice, DescriptorManager* descriptorManager, CommandManager* commandManager) override;
	void Resize(int w, int h) override;
	void RecordCommands() override;
	void CreatePipeline() override; // Managing chain for 3D elements (data not draw)

	GraphicsPipeline GetGraphicsPipeline() { return m_graphicsPipeline; };

	void Release();

	//camera
	void UpdateTemp();

private:
	HWND m_windowWP;

	GraphicsDevice* mp_graphicsDevice;
	DescriptorManager* mp_descriptorManager;
	CommandManager* mp_commandManager;

	GraphicsPipeline m_graphicsPipeline;
	MeshManager m_meshManager;

	//Camera (temp)
	CameraManager* mp_cameraManager;           // ← ta caméra
	ComPtr<ID3D12Resource> m_cameraCB;  // ← constant buffer pour la caméra
	void CreateCameraBuffer();
};

