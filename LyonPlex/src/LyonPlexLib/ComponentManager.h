#pragma once

#include "EntityManager.h"
#include "Components.h"


class ComponentManager 
{
public:
    template<typename T>
    void AddComponent(Entity entity, T* component)
    {
        auto& storage = m_components[entity.id];
        assert((storage.mask & component->mask) == 0 && "Component already exists");
        //if ((storage.mask & component->mask) == 1)
        //    return;
        storage.mask |= component->mask;
        storage.data.push_back(component);
    }

    template<typename T>
    void RemoveComponent(Entity entity) 
    {
        auto& storage = m_components[entity.id];
        for (size_t i = 0; i < storage.data.size(); ++i) 
        {
            if (storage.data[i]->typeID == T::StaticTypeID) 
            {
                storage.mask &= ~storage.data[i]->mask;
                delete storage.data[i];
                storage.data.erase(storage.data.begin() + i);
                return;
            }
        }
    }
    template<typename T>
    T* GetComponent(Entity entity) const 
    {
        auto it = m_components.find(entity.id);
        if (it == m_components.end()) return nullptr;
        for (auto* comp : it->second.data) {
            if (comp->typeID == T::StaticTypeID)
                return static_cast<T*>(comp);
        }
        return nullptr;
    }


    const std::vector<Component*>& GetComponents(Entity entity) const;

    ComponentMask GetMask(Entity entity) const;

    void RemoveAllComponents(Entity entity);

    void ClearAllEntityComponents(); 

    void DestroyEntity(Entity entity);

private:
    struct Storage 
    {
        ComponentMask mask = 0;
        std::vector<Component*> data;

        void Clear() 
        {
            for (auto* comp : data) delete comp;
            data.clear();
            mask = 0;
        }
    };
    std::unordered_map<uint32_t, Storage> m_components;
};
