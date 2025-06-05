#pragma once
class DescriptorManager
{
public:
	bool Init(GraphicsDevice* graphicsDevice);

	void CreateDescriptors();

    void CreateDepthStencilFormat(int i);

	ComPtr<ID3D12DescriptorHeap>& GetRtvHeap(){ return m_rtvHeap; };
	UINT& GetRtvDescriptorSize() { return m_rtvDescriptorSize; };
	UINT& GetDsvDescriptorSize() { return m_dsvDescriptorSize; };

    // Appelees une seule fois a l’init
    void CreateRtvHeap(UINT numDescriptors);
    void CreateSrvHeap(UINT numDescriptors);
    void CreateDsvHeap(UINT numDescriptors);
    void CreateSamplerHeap(UINT numDescriptors);


    // Allocation CPU/GPU pour chaque type
    D3D12_CPU_DESCRIPTOR_HANDLE AllocateSrvCPU() { return AllocateCPU(m_srvHeap, m_srvDescriptorSize, m_srvNextOffset); }
    D3D12_GPU_DESCRIPTOR_HANDLE AllocateSrvGPU() { return AllocateGPU(m_srvHeap, m_srvDescriptorSize, m_srvNextOffset); }
    D3D12_CPU_DESCRIPTOR_HANDLE AllocateRtvCPU() { return AllocateCPU(m_rtvHeap, m_rtvDescriptorSize, m_rtvNextOffset); }
    D3D12_CPU_DESCRIPTOR_HANDLE AllocateDsvCPU() { return AllocateCPU(m_dsvHeap, m_dsvDescriptorSize, m_dsvNextOffset); }
    D3D12_CPU_DESCRIPTOR_HANDLE AllocateSamplerCPU() { return AllocateCPU(m_samplerHeap, m_samplerDescriptorSize, m_samplerNextOffset); }

    ID3D12DescriptorHeap* GetSrvHeap()     const { return m_srvHeap.Get(); }
    ID3D12DescriptorHeap* GetRtvHeap()     const { return m_rtvHeap.Get(); }
    ID3D12DescriptorHeap* GetDsvHeap()     const { return m_dsvHeap.Get(); }
    ID3D12DescriptorHeap* GetSamplerHeap() const { return m_samplerHeap.Get(); }

    void ResetSrv() { m_srvNextOffset = 0; }
    void ResetRtv() { m_rtvNextOffset = 0; }
    void ResetDsv() { m_dsvNextOffset = 0; }
    void ResetSampler() { m_samplerNextOffset = 0; }

	void Release();

private:

    // Methode generique interne
    void CreateHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool shaderVisible, ComPtr<ID3D12DescriptorHeap>& heap, UINT& descriptorSize, UINT& nextOffset);

    D3D12_CPU_DESCRIPTOR_HANDLE AllocateCPU(ComPtr<ID3D12DescriptorHeap>& heap, UINT descriptorSize, UINT& nextOffset);

    D3D12_GPU_DESCRIPTOR_HANDLE AllocateGPU(ComPtr<ID3D12DescriptorHeap>& heap, UINT descriptorSize, UINT& nextOffset);

    GraphicsDevice* mp_graphicsDevice;

    // Resources DSV
    std::vector<ComPtr<ID3D12Resource>> m_depthResources;

    // Membres distincts
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap; // TAS de descripteurs pour les render target views (RTV)
    ComPtr<ID3D12DescriptorHeap> m_srvHeap;
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
    ComPtr<ID3D12DescriptorHeap> m_samplerHeap;

    UINT m_srvDescriptorSize, m_rtvDescriptorSize, m_dsvDescriptorSize, m_samplerDescriptorSize;
    UINT m_srvNextOffset, m_rtvNextOffset, m_dsvNextOffset, m_samplerNextOffset;
};


