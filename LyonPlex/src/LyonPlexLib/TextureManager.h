#pragma once
#include "ResourceManager.h"
#include "GraphicsDevice.h"
#include <wrl.h>
#include <d3d12.h>
#include <string>
#include <vector>

using Microsoft::WRL::ComPtr;

//-----------------------------------------------------------------------------//
// Texture data struct: holds GPU resource and SRV descriptor handle
//-----------------------------------------------------------------------------//
struct TextureData {
    ComPtr<ID3D12Resource>       resource;      // GPU texture resource
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;     // SRV description
    // Optionally: store descriptor heap index or handle
    size_t                       descriptorIndex;

    // Called by ResourceManager when loading
    bool LoadFromFile(const std::string& path, GraphicsDevice* device,
        DescriptorManager* descMgr) {
        // Use helper to load WIC texture into default heap
        ComPtr<ID3D12Resource> uploadHeap;
        HRESULT hr = DirectX::CreateWICTextureFromFile(
            device->GetDevice(), device->GetCommandQueue(),
            std::wstring(path.begin(), path.end()).c_str(),
            &resource, &uploadHeap
        );
        if (FAILED(hr)) return false;

        // Create SRV in a shader-visible descriptor heap
        auto srvHandle = descMgr->AllocateSrvGPU();
        device->GetDevice()->CreateShaderResourceView(
            resource.Get(), nullptr, srvHandle
        );

        // Store index (optional)
        descriptorIndex = descMgr->CurrentSrvIndex() - 1;

        return true;
    }

    void Unload() {
        resource.Reset();
        // descriptor cleanup if needed
    }
};

//-----------------------------------------------------------------------------//
// TextureManager using ResourceManager<TextureData>
//-----------------------------------------------------------------------------//
class TextureManager {
public:
    using TextureID = ResourceManager<TextureData, std::string>::ResourceID;

    TextureManager(GraphicsDevice* device, DescriptorManager* descMgr)
        : m_device(device), m_descMgr(descMgr) {}

    // Load or retrieve existing texture
    TextureID LoadTexture(const std::string& key) {
        if (m_textureLib.Has(key))
            return m_textureLib.Load(key); // returns existing

        // Create new entry then call custom LoadFromFile
        auto id = m_textureLib.Add(TextureData{});
        auto& texData = m_textureLib.Get(id);
        if (!texData.LoadFromFile(key, m_device, m_descMgr)) {
            m_textureLib.Remove(id);
            throw std::runtime_error("Failed to load texture: " + key);
        }
        return id;
    }

    // Retrieve SRV GPU handle for binding
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandle(TextureID id) const {
        size_t idx = m_textureLib.Get(id).descriptorIndex;
        return m_descMgr->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart() +
            idx * m_descMgr->GetSrvDescriptorSize();
    }

private:
    ResourceManager<TextureData, std::string> m_textureLib;
    GraphicsDevice* m_device;
    DescriptorManager* m_descMgr;
};

















//-----------------------------------------------------------------------------//
// SOLUCE 2
//-----------------------------------------------------------------------------//


#pragma once

#include "ResourceManager.h"
#include "GraphicsDevice.h"
#include "DescriptorManager.h"
#include "WICTextureLoader.h"   // DirectXTK helper
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
    bool LoadFromFile(const std::string& path,
        GraphicsDevice* device,
        DescriptorManager* descMgr) {
        // Convert path to wide string
        std::wstring wpath(path.begin(), path.end());
        // Create texture resource (default heap) + upload heap
        ComPtr<ID3D12Resource> uploadHeap;
        HRESULT hr = DirectX::CreateWICTextureFromFile(
            device->GetDevice(),
            device->GetCommandQueue(),
            wpath.c_str(),
            &resource,
            &uploadHeap
        );
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
    TextureID LoadTexture(const std::string& key) {
        if (m_textureLib.Has(key))
            return m_textureLib.Load(key);

        // Add placeholder, then actually load
        auto id = m_textureLib.Add(TextureData{});
        auto& texData = m_textureLib.Get(id);
        if (!texData.LoadFromFile(key, m_device, m_descMgr)) {
            m_textureLib.Remove(id);
            throw std::runtime_error("Failed to load texture: " + key);
        }
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
