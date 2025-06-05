#include "pch.h"
#include "CameraSystem.h"

void CameraSystem::InitRenderVariables(GraphicsDevice* device, CommandManager* cmdMgr, Render3D* r3d)
{
	mp_graphicsDevice = device;
	mp_commandManager = cmdMgr;
	mp_render3d = r3d;
}

void CameraSystem::Init(ECSManager& ecs)
{
	// Creer un seul buffer upload pour la camera (view+proj)
	UINT64 sizeCB = (sizeof(CBPerCamera) + 255) & ~255;
	CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(sizeCB);

	HRESULT hr = mp_graphicsDevice->GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&m_cameraConstantBuffer)
	);
	assert(SUCCEEDED(hr));

	// On mappe une fois pour pouvoir faire memcpy plus tard
	CD3DX12_RANGE readRange(0, 0);
	hr = m_cameraConstantBuffer->Map(
		0, &readRange,
		reinterpret_cast<void**>(&m_mappedCBData)
	);
	assert(SUCCEEDED(hr));
}

void CameraSystem::Update(ECSManager& ecs, float dt)
{
	// 1) On recherche l'entite qui a un CameraComponent.
	//    (On suppose qu’il n’y a qu’une seule entite “camera” active.)
	ComponentMask camMask = (1ULL << CameraComponent::StaticTypeID | 1ULL << TransformComponent::StaticTypeID);
	ecs.ForEach(camMask, [&](Entity e) {
		auto* cam = ecs.GetComponent<CameraComponent>(e);
		auto* tComp = ecs.GetComponent<TransformComponent>(e);
		if (!cam) return;

		// 2) ici la liaison entre la pos et orientation
        {
            if (tComp->parent.id != UINT32_MAX)
            {
                Entity eParent;
                eParent.id = tComp->parent.id;
                auto* tParent = ecs.GetComponent<TransformComponent>(eParent);

                cam->position = tParent->position;
                cam->position.x += tComp->position.x;
                cam->position.y += tComp->position.y;
                cam->position.z += tComp->position.z;
            }
            else
            {
                // Copier la position directement
                cam->position = tComp->position;
            }

            // Charger le quaternion de rotation
            XMVECTOR qRot = XMLoadFloat4(&tComp->rotation);

            // Vecteurs de base (dans l'espace local de la caméra)
            static const XMVECTOR defaultForward = XMVectorSet(0.f, 0.f, 1.f, 0.f);
            static const XMVECTOR defaultUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
            static const XMVECTOR defaultRight = XMVectorSet(1.f, 0.f, 0.f, 0.f);

            // Tourner ces vecteurs par le quaternion
            XMVECTOR vForward = XMVector3Rotate(defaultForward, qRot);
            XMVECTOR vUp = XMVector3Rotate(defaultUp, qRot);
            XMVECTOR vRight = XMVector3Rotate(defaultRight, qRot);

            // Stocker les résultats dans le CameraComponent
            XMStoreFloat3(&cam->forward, vForward);
            XMStoreFloat3(&cam->up, vUp);
            XMStoreFloat3(&cam->right, vRight);

            // Indiquer que la vue doit être recalculée
            cam->viewDirty = true;
        }


		// 3) Si necessaire, recalculer la matrice view
		if (cam->viewDirty) {
			RecomputeViewMatrix(cam);
			cam->viewDirty = false;
		}
		// 4) Si necessaire, recalculer la matrice projection
		if (cam->projectionDirty) {
			RecomputeProjectionMatrix(cam);
			cam->projectionDirty = false;
		}

		// 5) Copier view et proj transposees dans le CB GPU
		CBPerCamera cbData;
		cbData.view = cam->viewMatrix;       // deja transposee
		cbData.projection = cam->projectionMatrix;  // deja transposee
		memcpy(m_mappedCBData, &cbData, sizeof(cbData));

		});
}

void CameraSystem::RecomputeViewMatrix(CameraComponent* cam)
{
	XMVECTOR pos = XMLoadFloat3(&cam->position);
	XMVECTOR forward = XMVector3Normalize(XMLoadFloat3(&cam->forward));
	XMVECTOR up = XMVector3Normalize(XMLoadFloat3(&cam->up));
	XMMATRIX view = XMMatrixLookToLH(pos, forward, up);
	view = XMMatrixTranspose(view); // HLSL attend column-major
	XMStoreFloat4x4(&cam->viewMatrix, view);
}

void CameraSystem::RecomputeProjectionMatrix(CameraComponent* cam)
{
	float fovRad = XMConvertToRadians(cam->fovDegrees);
	XMMATRIX proj = XMMatrixPerspectiveFovLH(
		fovRad,
		cam->aspectRatio,
		cam->nearPlane,
		cam->farPlane
	);
	proj; // transpose pour HLSL
	proj = XMMatrixTranspose(proj);
	XMStoreFloat4x4(&cam->projectionMatrix, proj);
}

void CameraSystem::HandleInputAndMove(CameraComponent* cam, float dt)
{
    //// Exemples de touches (Z,S,D,Q,Espace/Ctrl) pour avancer/reculer/droite/gauche/haut/bas
    //if (GetAsyncKeyState('Z') & 0x8000) {
    //    XMVECTOR p = XMLoadFloat3(&cam->position);
    //    XMVECTOR f = XMLoadFloat3(&cam->forward);
    //    p = XMVectorAdd(p, XMVectorScale(f, cam->cameraSpeed * dt));
    //    XMStoreFloat3(&cam->position, p);
    //    cam->viewDirty = true;
    //}
    //if (GetAsyncKeyState('S') & 0x8000) {
    //    XMVECTOR p = XMLoadFloat3(&cam->position);
    //    XMVECTOR f = XMLoadFloat3(&cam->forward);
    //    p = XMVectorSubtract(p, XMVectorScale(f, cam->cameraSpeed * dt));
    //    XMStoreFloat3(&cam->position, p);
    //    cam->viewDirty = true;
    //}
    //if (GetAsyncKeyState('D') & 0x8000) {
    //    XMVECTOR p = XMLoadFloat3(&cam->position);
    //    XMVECTOR r = XMLoadFloat3(&cam->right);
    //    p = XMVectorAdd(p, XMVectorScale(r, cam->cameraSpeed * dt));
    //    XMStoreFloat3(&cam->position, p);
    //    cam->viewDirty = true;
    //}
    //if (GetAsyncKeyState('Q') & 0x8000) {
    //    XMVECTOR p = XMLoadFloat3(&cam->position);
    //    XMVECTOR r = XMLoadFloat3(&cam->right);
    //    p = XMVectorSubtract(p, XMVectorScale(r, cam->cameraSpeed * dt));
    //    XMStoreFloat3(&cam->position, p);
    //    cam->viewDirty = true;
    //}
    //if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
    //    XMVECTOR p = XMLoadFloat3(&cam->position);
    //    XMVECTOR u = XMLoadFloat3(&cam->up);
    //    p = XMVectorAdd(p, XMVectorScale(u, cam->cameraSpeed * dt));
    //    XMStoreFloat3(&cam->position, p);
    //    cam->viewDirty = true;
    //}
    //if (GetAsyncKeyState(VK_CONTROL) & 0x8000) {
    //    XMVECTOR p = XMLoadFloat3(&cam->position);
    //    XMVECTOR u = XMLoadFloat3(&cam->up);
    //    p = XMVectorSubtract(p, XMVectorScale(u, cam->cameraSpeed * dt));
    //    XMStoreFloat3(&cam->position, p);
    //    cam->viewDirty = true;
    //}

    //// Gestion de la souris pour Yaw/Pitch
    //if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
    //    if (!cam->rotating) {
    //        // On commence a faire tourner
    //        cam->rotating = true;
    //        POINT cursor;
    //        GetCursorPos(&cursor);
    //        ScreenToClient(GetActiveWindow(), &cursor);
    //        cam->prevMousePos = cursor;
    //    }
    //    else {
    //        // On calcule le deplacement depuis la frame precedente
    //        POINT cursor;
    //        GetCursorPos(&cursor);
    //        ScreenToClient(GetActiveWindow(), &cursor);
    //        int dx = cursor.x - cam->prevMousePos.x;
    //        int dy = cursor.y - cam->prevMousePos.y;

    //        float yawAngle = dx * cam->mouseSensitivity;
    //        float pitchAngle = dy * cam->mouseSensitivity;

    //        // Yaw : tourner autour de l’axe Y global
    //        {
    //            XMMATRIX R = XMMatrixRotationY(yawAngle);
    //            XMVECTOR f = XMVector3TransformNormal(XMLoadFloat3(&cam->forward), R);
    //            XMVECTOR r = XMVector3TransformNormal(XMLoadFloat3(&cam->right), R);
    //            XMStoreFloat3(&cam->forward, XMVector3Normalize(f));
    //            XMStoreFloat3(&cam->right, XMVector3Normalize(r));
    //            // Recalcule l’up pour rester orthogonal
    //            XMVECTOR u = XMVector3Cross(XMLoadFloat3(&cam->up), XMLoadFloat3(&cam->right));
    //            XMStoreFloat3(&cam->up, u);
    //        }

    //        // Pitch : tourner autour de l’axe “right” local
    //        {
    //            XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&cam->right), pitchAngle);
    //            XMVECTOR f = XMVector3TransformNormal(XMLoadFloat3(&cam->forward), R);
    //            XMVECTOR u = XMVector3TransformNormal(XMLoadFloat3(&cam->up), R);
    //            XMStoreFloat3(&cam->forward, XMVector3Normalize(f));
    //            XMStoreFloat3(&cam->up, XMVector3Normalize(u));
    //            // Recalcule “right” pour rester orthogonal
    //            XMVECTOR rNew = XMVector3Cross(XMLoadFloat3(&cam->up), XMLoadFloat3(&cam->forward));
    //            XMStoreFloat3(&cam->right, XMVector3Normalize(rNew));
    //        }

    //        cam->prevMousePos = cursor;
    //        cam->viewDirty = true;
    //    }
    //}
    //else {
    //    cam->rotating = false;
    //}
}
