#pragma once

#include "IRender.h"
#include "GraphicsPipeline.h"

struct Vertex {
	XMFLOAT3 position;
	XMFLOAT4 color;
};

class Render3D : public IRender
{
public:
	bool Init(HWND windowHandle, GraphicsDevice* graphicsDevice, DescriptorManager* descriptorManager, CommandManager* commandManager) override;
	void Resize(int w, int h) override;
	void RecordCommands() override;
	void CreatePipeline() override; // Managing chain for 3D elements (data not draw)

	GraphicsPipeline GetGraphicsPipeline() { return m_graphicsPipeline; };

private:
	GraphicsDevice* mp_graphicsDevice;
	DescriptorManager* mp_descriptorManager;
	CommandManager* mp_commandManager;
	HWND m_windowWP;

	GraphicsPipeline m_graphicsPipeline;

	ComPtr<ID3D12Resource> m_renderTargets[2];			//A changer ?

	// mesh
	ComPtr<ID3D12Resource> m_vertexBuffer;				// Buffer contenant les sommets à dessiner
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;		// Vue permettant d’accéder au vertex buffer dans le pipeline
};

