#include "pch.h"
#include "ECSManager.h"

void ECSManager::DestroyEntity(Entity e)
{
    assert(m_entityMgr.Exists(e) && "Entity must exist to destroy");
    m_destroyQueue.push_back(e);
}