#pragma once

//struct Entity
//{
//	int32_t id = 0;
//	uint32_t tab_index = 0;
//
//	int m_tag;
//	//bool m_isDestroyed = false;
//};
//
//class EntityManager
//{
//public:
//	~EntityManager();
//
//	Entity* CreateEntity();
//
//	void ResetEntitiesToAdd() { m_entitiesToAddIndex = 0; }
//
//	void ToDestroy(Entity* entity);
//	void DestroyEntity(Entity* entity);
//
//	Entity* (&GetEntityTab())[64000] { return m_tab_entity; }
//	std::vector<Entity>& GetToDestroyTab() { return m_vec_toDestroy; }
//	std::vector<Entity*>& GetToAddTab() { return m_vec_ToAdd; }
//
//	uint32_t GetNbEntity() { return m_entityNb; }
//
//
//private:
//	uint32_t m_entityNb = 0; // Positif, ce sont les entity qui existent actuellement dans le jeu
//	int32_t m_entitiesToAddIndex = 0; // Negatif pour savoir qu'elles ne sont pas encore ajoutees 
//
//	Entity* m_tab_entity[64000] = { nullptr };
//	std::vector<Entity> m_vec_toDestroy;
//	std::vector<Entity*> m_vec_ToAdd;
//};




//-----------------------------------------------------------------------------//
// Entity definition
//-----------------------------------------------------------------------------//
struct Entity
{
    uint32_t id;
};

//-----------------------------------------------------------------------------//
// SparseSet-based EntityManager
//-----------------------------------------------------------------------------//
class EntityManager 
{
public:
    EntityManager(size_t maxEntities = 1024);

    Entity Create();

    void Destroy(Entity e);

    bool Exists(Entity e) const;

    const std::vector<uint32_t>& GetAll() const { return m_dense; }

private:
    uint32_t m_nextId = 0;
    std::vector<uint32_t> m_dense;
    std::vector<uint32_t> m_sparse;
    std::vector<uint32_t> m_freeList;
};