#pragma once

class RenderingManager
{
public:
	void Init();
	
	void SetWindowHandle(HWND windowHandle) { m_createdWindow = windowHandle; }

	void CreatePipeline(); // Managing chain for 3D and 2D elements (data not draw)
	void RecordCommands(); // Enregistre dans la CommandList les instructions de rendu pour chaque frame (par exemple un draw)
	void ExecuteCommands(); // Envoie la CommandList enregistrée au GPU via la CommandQueue
	void Present(); // Demande à la swap chain de présenter le back buffer courant à l’écran
	void SignalAndWait(); // Synchronise le CPU et la GPU en signalant la fence et en attendant sa complétion

private:
	
	HWND m_createdWindow; // The created base window (pour le swapchain)
	const UINT FrameCount = 2; // Nombre de buffers utilisés dans le swap chain (double buffering)

	// COM objects
	ComPtr<ID3D12Device> m_device; // Interface principale pour interagir avec le GPU
	ComPtr<IDXGISwapChain3> m_swapChain; // Permet d’afficher les images rendues à l’écran
	ComPtr<ID3D12CommandQueue> m_commandQueue; // File d’attente pour les commandes GPU
	ComPtr<ID3D12DescriptorHeap> m_rtvHeap; // TAS de descripteurs pour les render target views (RTV)
	ComPtr<ID3D12Resource> m_renderTargets[2]; // Les textures (back buffers) où le rendu est effectué (taille FrameCount)
	ComPtr<ID3D12CommandAllocator> m_commandAllocator; // Alloue de la mémoire pour les commandes GPU
	ComPtr<ID3D12GraphicsCommandList> m_commandList; // Liste contenant les commandes graphiques à exécuter
	ComPtr<ID3D12Fence> m_fence; // Synchronisation CPU <-> GPU
	UINT m_rtvDescriptorSize; // Taille d’un descripteur RTV dans le heap
	UINT m_frameIndex; // Index du back buffer courant dans le swap chain
	HANDLE m_fenceEvent; // Événement Win32 pour attendre que la fence atteigne une valeur
	UINT64 m_fenceValue = 0; // Valeur actuelle de synchronisation de la fence

	// Pipeline graphique
	ComPtr<ID3D12RootSignature> m_rootSignature; // Décrit les ressources accessibles par les shaders
	ComPtr<ID3D12PipelineState> m_pipelineState; // Contient l'état complet du pipeline graphique
	ComPtr<ID3D12Resource> m_vertexBuffer; // Buffer contenant les sommets à dessiner
	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView; // Vue permettant d’accéder au vertex buffer dans le pipeline
};

