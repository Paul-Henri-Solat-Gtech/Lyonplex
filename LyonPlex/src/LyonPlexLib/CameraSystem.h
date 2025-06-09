#pragma once
#include "ECSManager.h"
//#include "ComponentManager.h"
//#include "CommandManager.h" // pour recuperer le command list
//#include "Render3D.h"      // pour acceder au root signature / pipeline state
//#include "GraphicsDevice.h"

#include <Windows.h>

class GraphicsDevice;
class CommandManager;
class Render3D;


struct CBPerCamera {
    XMFLOAT4X4 view;       // matrice view deja transposee
    XMFLOAT4X4 projection; // matrice proj deja transposee
};

class CameraSystem : public ISystem {
public:
    CameraSystem() {}

    void InitRenderVariables(GraphicsDevice* device, CommandManager* cmdMgr, Render3D* r3d);

    // Appele une seule fois apres avoir ajoute le composant a ECS
    virtual void Init(ECSManager& ecs) override;

    // Appele chaque frame, avant le rendu
    virtual void Update(ECSManager& ecs, float dt) override;

    // Aucun rendu direct dans le CameraSystem
    virtual void Render(ECSManager& ecs) override { }

    ComPtr<ID3D12Resource>& GetCBbuffer() { return m_cameraConstantBuffer; }


private:
    GraphicsDevice* mp_graphicsDevice = nullptr;
    CommandManager* mp_commandManager = nullptr;
    Render3D* mp_render3d = nullptr;

    ComPtr<ID3D12Resource> m_cameraConstantBuffer;
    void* m_mappedCBData = nullptr;

    // Remplit cam->viewMatrix (XMFLOAT4X4) en fonction de cam->position / cam->forward / cam->up
    void RecomputeViewMatrix(CameraComponent* cam);

    // Remplit cam->projectionMatrix en fonction de cam->fov / aspectRatio / near / far
    void RecomputeProjectionMatrix(CameraComponent* cam);

    // Gere l’input clavier/souris et met a jour position/forward/right/up, puis marque viewDirty
    void HandleInputAndMove(CameraComponent* cam, float dt);
    
};
