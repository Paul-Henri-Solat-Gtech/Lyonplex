#pragma once

#include "IRender.h"
#include "GraphicsPipeline.h"
#include "MeshManager.h"

inline UINT Align256(UINT size) { return (size + 255) & ~255; } //	Mettre dans Utils ??

struct ConstantBuffData
{
	DirectX::XMFLOAT4X4 World;		// Pos objet
	//DirectX::XMFLOAT4X4 ViewProj;	// Projection par rapport a la camera ? (mieux de ne pas combiner cote CPU (en code) pour laisser faire dans le shader (GPU?))
};

class Render3D : public IRender
{
public:
	bool Init(HWND windowHandle, ECSManager* ECS, GraphicsDevice* graphicsDevice, DescriptorManager* descriptorManager, CommandManager* commandManager) override;
	
	
	void Resize(int w, int h) override;
	void RecordCommands() override;
	void CreatePipeline() override; // Managing chain for 3D elements (data not draw)

	GraphicsPipeline& GetGraphicsPipeline() { return m_graphicsPipeline; };


	bool InitConstantBuffer();
	void UpdateCbParams();
	void UpdateAndBindCB(Entity ent);
	
	void Release();

private:
	HWND				m_windowWP;

	GraphicsDevice*		mp_graphicsDevice;
	DescriptorManager*	mp_descriptorManager;
	CommandManager*		mp_commandManager;

	GraphicsPipeline	m_graphicsPipeline;
	MeshManager			m_meshManager;

	//ECS Manager
	ECSManager*			m_ECS;

	// Data linked to cBuffer VertexParam
	ComPtr<ID3D12Resource>	m_cbTransformUpload = nullptr;
	void*					m_mappedCBData = nullptr;
	UINT					m_cbSize = Align256(sizeof(ConstantBuffData)); // taille alignee a 256

	// TEST
	UINT m_entityCount = 0;
	UINT m_frameCount = 0;
	UINT totalSize = 0;
	
};

