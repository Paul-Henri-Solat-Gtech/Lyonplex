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

void ComponentManager::ClearAllEntityComponents()
{
    for (auto& storage : m_components)
    {
        storage.second.Clear();
    }
    //m_components.clear();     // ==> PAS necessaire puisque EntityManager r�utilise les ID des entit�s supprim�es, donc ne pas clear() le tableau fait gagner du temps de non-r�allocation des cases m�moire
}

const std::vector<Component*>& ComponentManager::GetComponents(Entity entity) const
{
    static const std::vector<Component*> empty;
    auto it = m_components.find(entity.id);
    if (it == m_components.end()) return empty;
    return it->second.data;
}
