#pragma once

class GraphicsDevice
{
public:
	bool Init(HWND windowWP);

	void CreateGraphicsDevice();

	ComPtr<ID3D12Device>& GetDevice() { return m_device; };
	ComPtr<ID3D12CommandQueue>& GetCommandQueue() { return m_commandQueue; };
	ComPtr<IDXGISwapChain3>& GetSwapChain() { return m_swapChain; };

	const UINT& const GetFrameCount() { return FRAMECOUNT; };
	UINT& GetFrameIndex() { return m_frameIndex; };

	void Release();

private:

	void CreateDevice();
	void CreateCommandQueue();
	void CreateSwapChain();

	HWND m_windowWP;			// The created base window (pour le swapchain)
	const UINT FRAMECOUNT = 2;  // Nombre de buffers utilisés dans le swap chain (double buffering)

	// COM objects
	ComPtr<ID3D12Device> m_device;						// Interface principale pour interagir avec le GPU
	ComPtr<ID3D12CommandQueue> m_commandQueue;			// File d’attente pour les commandes GPU
	ComPtr<IDXGISwapChain3> m_swapChain;				// Permet d’afficher les images rendues à l’écran

	// Device
	ComPtr<IDXGIFactory6> m_factory;
	UINT m_frameIndex;
};

