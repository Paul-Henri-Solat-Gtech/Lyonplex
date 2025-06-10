#pragma once

#include "ResourceManager.h"
#include "GraphicsDevice.h"
#include "DescriptorManager.h"
#include "WICTextureLoader.h"   // DirectXTK helper
#include "ResourceUploadBatch.h"   // DirectXTK helper
#include <wrl.h>
#include <d3d12.h>
#include <string>

using Microsoft::WRL::ComPtr;

//-----------------------------------------------------------------------------//
// TextureData: holds GPU resource + SRV descriptor index
//-----------------------------------------------------------------------------//
struct TextureData {
    ComPtr<ID3D12Resource>       resource;        // GPU texture
    size_t                       descriptorIndex; // index in SRV heap

    // Called by ResourceManager when loading
    bool LoadFromFile(const std::string& path, GraphicsDevice* device, DescriptorManager* descMgr) 
    {
        ResourceUploadBatch uploadBatch(m_device.Get());
        uploadBatch.Begin();

        ComPtr<ID3D12Resource> texture;
        HRESULT hr = CreateWICTextureFromFile(
            m_device.Get(),
            uploadBatch,
            L"texture.png",
            &texture,
            true // mipmaps
        );

        // SRV
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = texture->GetDesc().Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = texture->GetDesc().MipLevels;

        CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv(m_srvHeap->GetCPUDescriptorHandleForHeapStart(), slotIndex, descriptorSize);
        m_device->CreateShaderResourceView(texture.Get(), &srvDesc, hCpuSrv);

        uploadBatch.End(m_commandQueue.Get()).wait();



        // Convert path to wide string
        std::wstring wpath(path.begin(), path.end());

        // Create texture resource (default heap) + upload heap
        ComPtr<ID3D12Resource> uploadHeap;
        HRESULT hr = DirectX::CreateWICTextureFromFile(device->GetDevice().Get(), device->GetCommandQueue().Get(), wpath.c_str(), &resource, &uploadHeap);
        if (FAILED(hr)) return false;

        // Create SRV in shader-visible heap
        D3D12_CPU_DESCRIPTOR_HANDLE srvCpu = descMgr->AllocateSrvCPU();
        D3D12_GPU_DESCRIPTOR_HANDLE srvGpu = descMgr->AllocateSrvGPU();
        device->GetDevice()->CreateShaderResourceView(
            resource.Get(),      // texture
            nullptr,             // use default SRV desc
            srvCpu
        );
        // Store index for GPU handle calculation
        descriptorIndex = descMgr->CurrentSrvOffset() - 1;
        return true;
    }

    void Unload() {
        resource.Reset();
        // Descriptor remains allocated but can be reused
    }
};

//-----------------------------------------------------------------------------//
// TextureManager: loads and caches textures via ResourceManager<TextureData>
//-----------------------------------------------------------------------------//
class TextureManager {
public:
    using TextureID = ResourceManager<TextureData, std::string>::ResourceID;

    TextureManager(GraphicsDevice* device, DescriptorManager* descMgr)
        : m_device(device), m_descMgr(descMgr) {}

    // Load or retrieve existing texture
    TextureID LoadTexture(const std::string& key)
    {
        // 1. Extraire l'extension en minuscules
        auto ext = std::filesystem::path(key).extension().string();
        std::for_each(ext.begin(), ext.end(), [](char& c) { c = static_cast<char>(std::tolower(c)); });

        // 2. Ajout d'une entrée dans le ResourceManager
        auto id = m_textureLib.Has(key) ? m_textureLib.Load(key) : m_textureLib.Add(TextureData{});
        auto& texData = m_textureLib.Get(id);

        // 3. Charger la texture selon le format
        ComPtr<ID3D12Resource> uploadHeap; // pour les copies upload
        HRESULT hr = S_OK;
        std::wstring wpath(key.begin(), key.end());

        if (ext == L".dds") 
        {
            hr = DirectX::CreateDDSTextureFromFile(m_device->GetDevice(), m_device->GetCommandQueue(), wpath.c_str(), &texData.resource, &uploadHeap);
        }
        else 
        {
            hr = DirectX::CreateWICTextureFromFile(m_device->GetDevice(), m_device->GetCommandQueue(), wpath.c_str(), &texData.resource, &uploadHeap);
        }
        if (FAILED(hr)) 
        {
            // Nettoyage en cas d'échec
            if (!m_textureLib.Has(key))
                m_textureLib.Remove(id);
            throw std::runtime_error("Failed to load texture: " + key);
        }

        // 4. Créer le SRV dans le heap CPU, puis stocker l'offset
        auto srvCpu = m_descMgr->AllocateSrvCPU();
        m_device->GetDevice()->CreateShaderResourceView(
            texData.resource.Get(),
            nullptr,    // descriptor par défaut
            srvCpu
        );
        texData.descriptorIndex = m_descMgr->CurrentSrvOffset() - 1;

        return id;
    }


    // Get GPU descriptor handle for SRV slot
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHandle(TextureID id) const {
        UINT offset = static_cast<UINT>(m_textureLib.Get(id).descriptorIndex);
        auto base = m_descMgr->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart();
        return { base.ptr + offset * m_descMgr->GetSrvDescriptorSize() };
    }

private:
    ResourceManager<TextureData, std::string> m_textureLib;
    GraphicsDevice* m_device;
    DescriptorManager* m_descMgr;
};
