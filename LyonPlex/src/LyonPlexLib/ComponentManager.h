#pragma once

//struct EntityComponents
//{
//	uint32_t tab_index = 0;
//	ComponentMask mask = 0;
//	std::vector<Component*> vec_m_components;
//
//	~EntityComponents()
//	{
//		for (auto* comp : vec_m_components)
//		{
//			delete comp;
//		}
//		vec_m_components.clear();
//	}
//};
//
//class ComponentManager
//{
//public:
//
//	EntityComponents* (&GetComponentsTab())[64000] { return tab_Components; }
//
//	std::vector<EntityComponents*>& GetComponentsToAddTab() { return tab_compToAdd; }
//
//private:
//	~ComponentManager();
//
//	EntityComponents* tab_Components[64000] = { nullptr };
//	std::vector<EntityComponents*> tab_compToAdd;
//};

#include "EntityManager.h"

using ComponentMask = uint64_t; // assume up to 64 component types


//-----------------------------------------------------------------------------//
// Entity definition
//-----------------------------------------------------------------------------//

//-----------------------------------------------------------------------------//
// Component storage
//-----------------------------------------------------------------------------//


struct Component {
    virtual ~Component() = default;
    ComponentMask mask;
    uint32_t typeID;
};

class ComponentManager 
{
public:
    template<typename T>
    void AddComponent(Entity entity, T* component)
    {
        auto& storage = m_components[entity.id];
        assert((storage.mask & component->mask) == 0 && "Component already exists");
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

    void RemoveAllComponents(Entity entity);

    ComponentMask GetMask(Entity entity) const
    {
        auto it = m_components.find(entity.id);
        if (it == m_components.end()) return 0;
        return it->second.mask;
    }

    const std::vector<Component*>& GetComponents(Entity entity) const
    {
        static const std::vector<Component*> empty;
        auto it = m_components.find(entity.id);
        if (it == m_components.end()) return empty;
        return it->second.data;
    }

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


