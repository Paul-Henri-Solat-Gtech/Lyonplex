#pragma once

#include "IRender.h"
#include "GraphicsPipeline.h"
#include "MeshManager.h"

class Render3D : public IRender
{
public:
	bool Init(HWND windowHandle, GraphicsDevice* graphicsDevice, DescriptorManager* descriptorManager, CommandManager* commandManager) override;
	void Resize(int w, int h) override;
	void RecordCommands() override;
	void CreatePipeline() override; // Managing chain for 3D elements (data not draw)

	GraphicsPipeline GetGraphicsPipeline() { return m_graphicsPipeline; };

	void Release();

private:
	GraphicsDevice* mp_graphicsDevice;
	DescriptorManager* mp_descriptorManager;
	CommandManager* mp_commandManager;
	HWND m_windowWP;

	GraphicsPipeline m_graphicsPipeline;
	MeshManager m_meshManager;


	// mesh
	ComPtr<ID3D12Resource> m_vertexBuffer;				// Buffer contenant les sommets à dessiner
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;		// Vue permettant d’accéder au vertex buffer dans le pipeline
};

