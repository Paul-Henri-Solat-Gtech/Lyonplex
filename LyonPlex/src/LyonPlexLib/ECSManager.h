#pragma once

#include "EntityManager.h"
#include "ComponentManager.h"

//class ECSManager
//{
//public:
//
//private:
//
//
//};



//-----------------------------------------------------------------------------//
// ECSManager: orchestration
//-----------------------------------------------------------------------------//
class ECSManager 
{
public:
    ECSManager(size_t maxEntities = 1024) : m_entityMgr(maxEntities) {}

    Entity CreateEntity() { return m_entityMgr.Create(); }

    /*void DestroyEntity(Entity entity) 
    {
        m_componentMgr.RemoveAllComponents(entity);
        m_entityMgr.Destroy(entity);
    }*/

    // Queue destruction to avoid modifying during iteration
    void DestroyEntity(Entity e) {
        assert(m_entityMgr.Exists(e) && "Entity must exist to destroy");
        m_destroyQueue.push_back(e);
    }

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

    ComponentMask GetComponentMask(Entity entity) const
    {
        return m_componentMgr.GetMask(entity);
    }

    // Example of a system: call func for each entity matching mask
    template<typename Func>
    void ForEach(ComponentMask mask, Func func) 
    {
        for (auto id : m_entityMgr.GetAll()) {
            Entity entity{ id };
            if ((m_componentMgr.GetMask(entity) & mask) == mask) {
                func(entity);
            }
        }
    }


    // Process all queued destructions at end of frame
    void EndFrame() 
    {
        for (Entity e : m_destroyQueue) {
            m_componentMgr.RemoveAllComponents(e);
            m_entityMgr.Destroy(e);
        }
        m_destroyQueue.clear();
    }

private:
    EntityManager m_entityMgr;
    ComponentManager m_componentMgr;

    std::vector<Entity> m_destroyQueue;
};
