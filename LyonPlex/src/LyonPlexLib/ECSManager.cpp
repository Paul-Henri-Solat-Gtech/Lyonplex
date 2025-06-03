#include "pch.h"
#include "ECSManager.h"

void ECSManager::Init(GraphicsDevice* device, CommandManager* cmdMgr, Render3D* r3d)
{
    m_systemMgr.InitManager(this, device, cmdMgr, r3d);
}

void ECSManager::DestroyEntity(Entity e)
{
    assert(m_entityMgr.Exists(e) && "Entity must exist to destroy");
    m_destroyQueue.push_back(e);
}