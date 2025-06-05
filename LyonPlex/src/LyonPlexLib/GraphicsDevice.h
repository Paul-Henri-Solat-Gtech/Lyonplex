#pragma once

//#include "DescriptorManager.h"
class DescriptorManager;

class GraphicsDevice
{
public:
	bool Init(HWND windowWP);

	void CreateGraphicsDevice();

	void SetRenderTargets(DescriptorManager* descManager);

	ComPtr<ID3D12Device>& GetDevice() { return m_device; };
	ComPtr<ID3D12CommandQueue>& GetCommandQueue() { return m_commandQueue; };
	ComPtr<IDXGISwapChain3>& GetSwapChain() { return m_swapChain; };
	//ComPtr<ID3D12Resource>& GetRenderTargets() { return m_renderTargets; };
	ID3D12Resource* GetRenderTargetResource(UINT i) const { return m_renderTargets[i].Get(); }

	CD3DX12_CPU_DESCRIPTOR_HANDLE& GetRtvHandle() { return m_rtvHandle; };

	const UINT& const GetFrameCount() { return FRAMECOUNT; };
	UINT& GetFrameIndex() { return m_frameIndex; };

	HWND GetWindow() { return m_windowWP; }

	void Present();

	void Release();

private:

	void CreateDevice();
	void CreateCommandQueue();
	void CreateSwapChain();


	HWND m_windowWP;			// The created base window (pour le swapchain)
	const UINT FRAMECOUNT = 2;  // Nombre de buffers utilises dans le swap chain (double buffering)

	ComPtr<IDXGIFactory6> m_factory;
	ComPtr<ID3D12Device> m_device;							// Interface principale pour interagir avec le GPU
	ComPtr<ID3D12CommandQueue> m_commandQueue;				// File d’attente pour les commandes GPU
	ComPtr<IDXGISwapChain3> m_swapChain;					// Permet d’afficher les images rendues a l’ecran
	
	std::vector<ComPtr<ID3D12Resource>> m_renderTargets;	// Nombre de RenderTargets (double/triple buffering)

	CD3DX12_CPU_DESCRIPTOR_HANDLE m_rtvHandle;

	UINT m_frameIndex;


};

