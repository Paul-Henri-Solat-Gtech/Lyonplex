#pragma once
class CommandManager
{
public:
	bool Init(GraphicsDevice* graphicsDevice, DescriptorManager* descriptorManager);

	void CreateCommandManager();

	void Begin();

	void End();

	void ExecuteCmdLists();

	void SignalAndWait();


	ComPtr<ID3D12CommandAllocator>& GetCommandAllocator() { return m_commandAllocator; };
	ComPtr<ID3D12GraphicsCommandList>& GetCommandList() { return m_commandList; };
	ComPtr<ID3D12Fence>& GetFence() { return m_fence; };

	HANDLE& GetFenceEvent() { return m_fenceEvent; };
	UINT64& GetFenceValue() { return m_fenceValue; };

	void Release();

private:
	void CreateCommandAllocator();
	void CreateCommandList();
	void CreateFence();

	GraphicsDevice* mp_graphicsDevice;
	DescriptorManager* mp_descriptorManager;

	ComPtr<ID3D12CommandAllocator> m_commandAllocator;  // Alloue de la memoire pour les commandes GPU
	ComPtr<ID3D12GraphicsCommandList> m_commandList;	// Liste contenant les commandes graphiques a executer
	ComPtr<ID3D12Fence> m_fence;						// Synchronisation CPU <-> GPU
	HANDLE m_fenceEvent;								// Évenement Win32 pour attendre que la fence atteigne une valeur
	UINT64 m_fenceValue = 0;							// Valeur actuelle de synchronisation de la fence
};

