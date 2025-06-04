#pragma once

class ECSManager;

class ISystem
{
public:
    virtual ~ISystem() = default;
    virtual void Init(ECSManager& ecs) { }
    virtual void Update(ECSManager& ecs, float dt) { }
    virtual void Render(ECSManager& ecs) { }
};