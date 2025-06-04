#include "pch.h"
#include "EntityManager.h"

EntityManager::EntityManager(size_t maxEntities) : m_sparse(maxEntities, UINT32_MAX)
{
    m_dense.reserve(maxEntities);
}

Entity EntityManager::Create()
{
    uint32_t id;
    if (!m_freeList.empty()) 
    {
        id = m_freeList.back();
        m_freeList.pop_back();
    }
    else 
    {
        id = m_nextId++;
        assert(id < m_sparse.size() && "Exceeded maximum number of entities");
    }
    m_sparse[id] = static_cast<uint32_t>(m_dense.size());
    m_dense.push_back(id);
    return Entity{ id };
}

void EntityManager::Destroy(Entity entity)
{
    uint32_t id = entity.id;
    assert(Exists(entity) && "Destroying non-existent entity");

    // swap-remove from m_dense array
    uint32_t index = m_sparse[id];
    uint32_t lastId = m_dense.back();
    m_dense[index] = lastId;
    m_sparse[lastId] = index;
    m_dense.pop_back();

    // mark m_sparse entry as invalid
    m_sparse[id] = UINT32_MAX;

    // recycle id
    m_freeList.push_back(id);
}

void EntityManager::ClearAllEntities()
{
    m_dense.clear();
    m_sparse.clear();
    m_freeList.clear();
}

bool EntityManager::Exists(Entity entity) const
{
    return entity.id < m_sparse.size() && m_sparse[entity.id] != UINT32_MAX;
}
