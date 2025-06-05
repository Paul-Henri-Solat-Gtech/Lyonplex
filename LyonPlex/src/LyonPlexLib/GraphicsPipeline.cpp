#include "pch.h"
#include "GraphicsPipeline.h"

void GraphicsPipeline::Init(GraphicsDevice* graphicsDevice, DescriptorManager* descriptorManager, CommandManager* commandManager)
{
    mp_graphicsDevice = graphicsDevice;
    mp_descriptorManager = descriptorManager;
    mp_commandManager = commandManager;
}

void GraphicsPipeline::CreatePipeline()
{
    CreateRootSignature();
    CompileShader();
    CreatePipelineStateObject();
}

void GraphicsPipeline::CreateRootSignature()
{
    // 0) shader with camera
    // 1) Definition des deux root parameters (slot b0 et b1)
    CD3DX12_ROOT_PARAMETER rootParams[2];
    rootParams[0].InitAsConstantBufferView(0); // <- b0 côte shader pour camera (view & proj)
    rootParams[1].InitAsConstantBufferView(1); // <- b1 côte shader pour transform (world)

    // Params supplementaires

    // 2) Construire la root signature
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters = _countof(rootParams);
    rootSigDesc.pParameters = rootParams;
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    // 3) Serialiser et creer
    ComPtr<ID3DBlob> signature, error;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    if (FAILED(hr))
    {
        // Si errorBlob n’est pas null, on peut afficher son contenu (c'est un ID3DBlob)
        if (error)
        {
            // Le message d’erreur est au format ASCII (car D3DCompile et D3D12Serialize renvoient des blobs d’erreur ASCII)
            const char* msg = reinterpret_cast<const char*>(error->GetBufferPointer());
            OutputDebugStringA("D3D12SerializeRootSignature failed:\n");
            OutputDebugStringA(msg);
            OutputDebugStringA("\n");
        }
        else
        {
            OutputDebugStringA("D3D12SerializeRootSignature failed: aucun error blob retourne.\n");
        }
        // Ici, vous pouvez choisir de retourner ou de planter proprement
        return;
    }
    mp_graphicsDevice->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
}

void GraphicsPipeline::CompileShader()
{
    // 2) Compilation des shaders

    //VERTEX SHADER
    HRESULT hr = D3DCompileFromFile(L"../../../src/LyonPlexLib/VertexShader.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &m_vsBlob, &m_errorBlob);

    if (FAILED(hr)) {
        if (m_errorBlob) {
            // Affiche la raison exacte dans une MessageBox ou via Debug
            std::string msg((char*)m_errorBlob->GetBufferPointer(), m_errorBlob->GetBufferSize());
            MessageBoxA(nullptr, msg.c_str(), "Shader Compilation Error", MB_OK | MB_ICONERROR);
        }
        throw std::runtime_error("VS compilation failed");
    }

    //PIXEL SHADER
    hr = D3DCompileFromFile(L"../../../src/LyonPlexLib/VertexShader.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &m_psBlob, &m_errorBlob);

    if (FAILED(hr)) {
        if (m_errorBlob) OutputDebugStringA((char*)m_errorBlob->GetBufferPointer());
        throw std::runtime_error("PS compilation failed");
    }
}

void GraphicsPipeline::CreatePipelineStateObject()
{
    // 3) Input layout
    D3D12_INPUT_ELEMENT_DESC layout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,   0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        // Ajouter params supplémentaires
    };

    // 4) PSO
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { layout, _countof(layout) };
    psoDesc.pRootSignature = m_rootSignature.Get();
    psoDesc.VS = { m_vsBlob->GetBufferPointer(), m_vsBlob->GetBufferSize() };
    psoDesc.PS = { m_psBlob->GetBufferPointer(), m_psBlob->GetBufferSize() };
    
    // ) Rasterizer State : on veut afficher la face extérieure, winding CCW = front
    D3D12_RASTERIZER_DESC rasterDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    rasterDesc.CullMode = D3D12_CULL_MODE_BACK;          // On élimine les faces arrière (celles qu'on ne veut pas voir)
    rasterDesc.FrontCounterClockwise = TRUE;             // CCW = face avant, CW = face arrière 
    rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;         // Remplir normalement
    rasterDesc.DepthClipEnable = TRUE;

    psoDesc.RasterizerState = rasterDesc;

    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    
    // 5) Depth-Stencil State : ON et test LESS pour un cube « normal »
    D3D12_DEPTH_STENCIL_DESC depthDesc = {};
    depthDesc.DepthEnable = TRUE;
    depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depthDesc.StencilEnable = FALSE; // Pas de stencil pour l'instant

    psoDesc.DepthStencilState = depthDesc;

    // Render Target & Sample
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    psoDesc.SampleDesc.Count = 1;

    // Ajouter params supplementaires

    mp_graphicsDevice->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
}
