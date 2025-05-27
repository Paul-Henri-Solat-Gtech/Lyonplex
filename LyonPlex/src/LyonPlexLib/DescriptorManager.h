#pragma once
class DescriptorManager
{
public:
	bool Init(GraphicsDevice* graphicsDevice);

	void CreateDescriptorManager();

	ComPtr<ID3D12DescriptorHeap>& GetRtvHeap(){ return m_rtvHeap; };
	UINT& GetRtvDescriptorSize() { return m_rtvDescriptorSize; };

	void Release();

private:
	void CreateRTVHeap();

	GraphicsDevice* mp_graphicsDevice;

	ComPtr<ID3D12DescriptorHeap> m_rtvHeap;				// TAS de descripteurs pour les render target views (RTV)
	UINT m_rtvDescriptorSize;
};

