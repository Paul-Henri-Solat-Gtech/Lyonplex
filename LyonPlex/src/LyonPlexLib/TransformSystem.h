#pragma once
#include <unordered_map>
#include "ECSManager.h"
#include "ISystem.h"
//#include "ComponentManager.h"
//#include "TransformComponent.h"
//#include <DirectXMath.h>

class TransformSystem : public ISystem
{
public:
    // Stocke la matrice monde finale (XMMATRIX) pour chaque entity
    std::unordered_map<uint32_t, XMMATRIX> worldMatrices;
    
    virtual void Init(ECSManager& ecs) override { }

    // Mise a jour a appeler une fois par frame (apres modif eventuelle de transforms)
    virtual void Update(ECSManager& ecs, float dt) override;

    virtual void Render(ECSManager& ecs) override { }

    std::unordered_map<uint32_t, XMMATRIX>& GetWorldMatrix() { return worldMatrices; }

private:
    // Calcule recursivement la matrice monde pour l’entite "entity"
    XMMATRIX ComputeWorldMatrix(Entity entity, ECSManager& ecs);
};