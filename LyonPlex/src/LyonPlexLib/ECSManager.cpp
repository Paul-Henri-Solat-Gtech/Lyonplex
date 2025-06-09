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

void ECSManager::ClearAllEntities()
{
    //m_entityMgr.ClearAllEntities();
    //m_componentMgr.ClearAllEntityComponents();
    
    for (auto id : m_entityMgr.GetAll())
    {
        Entity e;
        e.id = id;
        DestroyEntity(e);
    }
}

void ECSManager::EndFrame()
{
    for (Entity e : m_destroyQueue)
    {
        m_componentMgr.RemoveAllComponents(e);
        m_entityMgr.Destroy(e);
    }
    m_destroyQueue.clear();
}
