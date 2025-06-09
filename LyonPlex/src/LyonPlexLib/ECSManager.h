#pragma once

#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"


//-----------------------------------------------------------------------------//
// ECSManager: orchestration
//-----------------------------------------------------------------------------//
class ECSManager 
{
public:
    ECSManager(size_t maxEntities = 1024) : m_entityMgr(maxEntities)  { }

    void Init(GraphicsDevice* device, CommandManager* cmdMgr, Render3D* r3d);

    Entity CreateEntity() { return m_entityMgr.Create(); }

    // Queue destruction to avoid modifying during iteration
    void DestroyEntity(Entity e);

    void ClearAllEntities();

    int GetEntityCount() { return m_entityMgr.GetEntityCount(); }

    // Add/remove m_components
    template<typename T>
    void AddComponent(Entity entity, T* comp)
    {
        assert(m_entityMgr.Exists(entity) && "Entity must exist");
        m_componentMgr.AddComponent<T>(entity, comp);
    }
    template<typename T>
    void RemoveComponent(Entity entity)
    {
        assert(m_entityMgr.Exists(entity) && "Entity must exist");
        m_componentMgr.RemoveComponent<T>(entity);
    }

    template<typename T>
    T* GetComponent(Entity e) const 
    {
        return m_componentMgr.GetComponent<T>(e);
    }

    ComponentMask GetComponentMask(Entity entity) const { return m_componentMgr.GetMask(entity); }

    // Example of a system: call func for each entity matching mask
    template<typename Func>
    void ForEach(ComponentMask mask, Func func) 
    {
        for (auto id : m_entityMgr.GetAll()) 
        {
            Entity entity{ id };
            if ((m_componentMgr.GetMask(entity) & mask) == mask) 
            {
                func(entity);
            }
        }
    }


    // Process all queued destructions at end of frame
    void EndFrame();

    SystemManager m_systemMgr;
private:
    EntityManager m_entityMgr;
    ComponentManager m_componentMgr;

    std::vector<Entity> m_destroyQueue;
};
