#include "pch.h"
#include "ComponentManager.h"

void ComponentManager::RemoveAllComponents(Entity entity)
{
    m_components[entity.id].Clear();
}

ComponentMask ComponentManager::GetMask(Entity entity) const
{
    auto it = m_components.find(entity.id);
    if (it == m_components.end()) return 0;
    return it->second.mask;
}

const std::vector<Component*>& ComponentManager::GetComponents(Entity entity) const
{
    static const std::vector<Component*> empty;
    auto it = m_components.find(entity.id);
    if (it == m_components.end()) return empty;
    return it->second.data;
}
