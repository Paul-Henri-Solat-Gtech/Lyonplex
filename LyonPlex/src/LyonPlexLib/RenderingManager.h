#pragma once

class RenderingManager
{
public:
	void Init();
	
	void SetWindowHandle(HWND windowHandle) { hwnd = windowHandle; }

	//Need to complete
	void RecordCommands();
	void ExecuteCommands();
	void Present();
	void SignalAndWait();
	void CreatePipeline();

private:
	
	HWND hwnd; // The created window
	const UINT FrameCount = 2;

	// COM objects
	ComPtr<ID3D12Device> device;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12Resource> renderTargets[2]; // a la place de 2 cest normalement FrameCount
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12Fence> fence;
	UINT rtvDescriptorSize;
	UINT frameIndex;
	HANDLE fenceEvent;
	UINT64 fenceValue = 0;

	//
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> pipelineState;
	ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
};

