#include "pch.h"
#include "ComponentManager.h"

void ComponentManager::RemoveAllComponents(Entity entity)
{
    m_components[entity.id].Clear();
}
