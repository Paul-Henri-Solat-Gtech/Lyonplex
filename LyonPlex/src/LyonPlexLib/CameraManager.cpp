#include "pch.h"
#include "CameraManager.h"

//bool CameraManager::Init(HWND windowWP, GraphicsDevice* graphicsDevice, CommandManager* commandManager, Render3D* render3d)
//{
//    mp_graphicsDevice = graphicsDevice;
//    mp_commandManager = commandManager;
//    mp_render3d = render3d;
//
//    m_position = { 0.f,0.f,-1.f };
//    m_right = { 1.f,0.f,0.f };
//    m_up = { 0.f,1.f,0.f };
//    m_forward = { 0.f,0.f,1.f };
//    
//    m_cameraSpeed = 0.2f;
//    m_mouseSensitivity = 0.005f;
//    m_rotating = false;
//    m_prevMousePos = { 0,0 };
//
//    // Change l'endroit de spawn de la camera
//    //SetPosition({ 0.0f, 0.0f, -2.0f });
//    //SetDirection({ 0.0f, 0.0f, 1.0f });
//
//    UpdateProjectionMatrix(90.0f, 800.f / 600.f, 0.1f, 1000.f);
//
//    m_viewNeedsUpdate = true;
//
//    return true;
//}
//
//void CameraManager::CreateCameraBuffer()
//{
//    UINT64 sizeCB = (sizeof(CameraBuffer) + 255) & ~255;
//    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
//    CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(sizeCB);
//
//    mp_graphicsDevice->GetDevice()->CreateCommittedResource(
//        &heapProps, D3D12_HEAP_FLAG_NONE, &desc,
//        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
//        IID_PPV_ARGS(&m_cameraConstantBuffer)
//    );
//}
//
//void CameraManager::UploadConstantBuffer()
//{
//    CameraBuffer cbData = GetCameraBuffer();
//    UINT8* mapped = nullptr;
//    CD3DX12_RANGE readRange(0, 0);
//    m_cameraConstantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&mapped));
//    memcpy(mapped, &cbData, sizeof(cbData));
//    m_cameraConstantBuffer->Unmap(0, nullptr);
//
//    // Bind CBV (slot b0) AVANT d'appeler Render3D.RecordCommands()
//    auto cmdList = mp_commandManager->GetCommandList();
//    cmdList->SetGraphicsRootSignature(mp_render3d->GetGraphicsPipeline().GetRootSignature().Get());
//    mp_commandManager->GetCommandList()->SetGraphicsRootConstantBufferView(0, m_cameraConstantBuffer->GetGPUVirtualAddress());
//}
//
//void CameraManager::UpdateViewMatrix()
//{
//    if (!m_viewNeedsUpdate) return;
//
//    XMVECTOR pos = XMLoadFloat3(&m_position);
//    XMVECTOR forward = XMVector3Normalize(XMLoadFloat3(&m_forward));
//    XMVECTOR up = XMVector3Normalize(XMLoadFloat3(&m_up));
//
//    XMMATRIX view = XMMatrixLookToLH(pos, forward, up);
//    XMStoreFloat4x4(&m_viewMatrix, view);
//
//    m_viewNeedsUpdate = false;
//}
//
//void CameraManager::UpdateProjectionMatrix(float fovDegrees, float aspectRatio, float nearPlane, float farPlane)
//{
//    float fovRadians = XMConvertToRadians(fovDegrees);
//    XMMATRIX proj = XMMatrixPerspectiveFovLH(fovRadians, aspectRatio, nearPlane, farPlane);
//    XMStoreFloat4x4(&m_projectionMatrix, proj);
//}
//
//CameraBuffer CameraManager::GetCameraBuffer() const
//{
//    // Charge la view et la projection
//    XMMATRIX viewMat = XMLoadFloat4x4(&m_viewMatrix);
//    XMMATRIX projMat = XMLoadFloat4x4(&m_projectionMatrix);
//
//    // Transpose en place pour HLSL (column-major)
//    viewMat = XMMatrixTranspose(viewMat);
//    projMat = XMMatrixTranspose(projMat);
//
//    return CameraBuffer{ viewMat, projMat };
//}
//
//void CameraManager::UpdateCamera()
//{
//    // Need a real input system
//        // ----------- Gestion clavier -----------
//    float deltaTime = 1.0f / 60.0f; // À remplacer par un vrai deltaTime si possible
//    if (GetAsyncKeyState('Z') & 0x8000) MoveForward(m_cameraSpeed * deltaTime);
//    if (GetAsyncKeyState('S') & 0x8000) MoveForward(-m_cameraSpeed * deltaTime);
//    if (GetAsyncKeyState('D') & 0x8000) MoveRight(m_cameraSpeed * deltaTime);
//    if (GetAsyncKeyState('Q') & 0x8000) MoveRight(-m_cameraSpeed * deltaTime);
//    if (GetAsyncKeyState(VK_SPACE) & 0x8000) MoveUp(m_cameraSpeed * deltaTime);
//    if (GetAsyncKeyState(VK_CONTROL) & 0x8000) MoveUp(-m_cameraSpeed * deltaTime);
//
//    // ----------- Gestion souris (clic droit + deplacement) -----------
//    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
//    {
//        // Si on vient juste d’appuyer, on initialise le point de depart
//        if (!m_rotating)
//        {
//            m_rotating = true;
//            POINT cursor;
//            GetCursorPos(&cursor);
//            ScreenToClient(m_windowWP, &cursor);
//            m_prevMousePos = cursor;
//        }
//        else
//        {
//            // On recupere la position actuelle et calcule le delta
//            POINT cursor;
//            GetCursorPos(&cursor);
//            ScreenToClient(m_windowWP, &cursor);
//
//            int dx = cursor.x - m_prevMousePos.x;
//            int dy = cursor.y - m_prevMousePos.y;
//
//            // Calcul des angles selon la sensibilite
//            float yawAngle = dx * m_mouseSensitivity;
//            float pitchAngle = dy * m_mouseSensitivity;
//
//            // Applique la rotation
//            Yaw(yawAngle);
//            Pitch(pitchAngle);
//
//            // Memorise la position pour la frame suivante
//            m_prevMousePos = cursor;
//        }
//    }
//    else
//    {
//        // Si on n’appuie plus, on desactive la rotation
//        m_rotating = false;
//    }
//
//    // ----------- Mettre a jour la matrice de vue -----------
//    UpdateViewMatrix();
//
//    // ----------- (facultatif) afficher dans le titre -----------
//    XMFLOAT3 pos = GetPosition();
//    wchar_t title[128];
//    swprintf(title, 128,
//        L"MonJeuDX12 - Camera [X: %.2f, Y: %.2f, Z: %.2f]",
//        pos.x, pos.y, pos.z);
//    ::SetWindowText(m_windowWP, title);
//}
//
//void CameraManager::SetPosition(const XMFLOAT3& pos)
//{
//    m_position = pos;
//    m_viewNeedsUpdate = true;
//}
//
//void CameraManager::SetDirection(const XMFLOAT3& forward)
//{
//    m_forward = forward;
//    m_viewNeedsUpdate = true;
//}
//
//void CameraManager::MoveForward(float distance)
//{
//    XMVECTOR pos = XMLoadFloat3(&m_position);
//    XMVECTOR dir = XMLoadFloat3(&m_forward);
//    pos = XMVectorAdd(pos, XMVectorScale(dir, distance));
//    XMStoreFloat3(&m_position, pos);
//    m_viewNeedsUpdate = true;
//}
//
//void CameraManager::MoveRight(float distance)
//{
//    XMVECTOR pos = XMLoadFloat3(&m_position);
//    XMVECTOR dir = XMLoadFloat3(&m_right);
//    pos = XMVectorAdd(pos, XMVectorScale(dir, distance));
//    XMStoreFloat3(&m_position, pos);
//    m_viewNeedsUpdate = true;
//}
//
//void CameraManager::MoveUp(float distance)
//{
//    XMVECTOR pos = XMLoadFloat3(&m_position);
//    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // vecteur vertical
//    pos = XMVectorAdd(pos, XMVectorScale(up, distance));
//    XMStoreFloat3(&m_position, pos);
//    m_viewNeedsUpdate = true;
//}
//
//void CameraManager::Yaw(float angleRadians)
//{
//    // On tourne forward et right autour de l’axe Y (up reste invariant)
//    XMMATRIX R = XMMatrixRotationY(angleRadians);
//
//    XMVECTOR f = XMVector3TransformNormal(XMLoadFloat3(&m_forward), R);
//    XMVECTOR r = XMVector3TransformNormal(XMLoadFloat3(&m_right), R);
//
//    XMStoreFloat3(&m_forward, XMVector3Normalize(f));
//    XMStoreFloat3(&m_right, XMVector3Normalize(r));
//
//    // Recalcule m_up pour garantir l’orthogonalite
//    XMVECTOR u = XMVector3Cross(XMLoadFloat3(&m_forward), XMLoadFloat3(&m_right));
//    XMStoreFloat3(&m_up, u);
//
//    m_viewNeedsUpdate = true;
//}
//
//void CameraManager::Pitch(float angleRadians)
//{
//    // On tourne forward et up autour de l’axe right
//    XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angleRadians);
//
//    XMVECTOR f = XMVector3TransformNormal(XMLoadFloat3(&m_forward), R);
//    XMVECTOR u = XMVector3TransformNormal(XMLoadFloat3(&m_up), R);
//
//    XMStoreFloat3(&m_forward, XMVector3Normalize(f));
//    XMStoreFloat3(&m_up, XMVector3Normalize(u));
//
//    // Recalcule m_right pour garantir l’orthogonalite
//    XMVECTOR r = XMVector3Cross(XMLoadFloat3(&m_up), XMLoadFloat3(&m_forward));
//    XMStoreFloat3(&m_right, XMVector3Normalize(r));
//
//    m_viewNeedsUpdate = true;
//}
