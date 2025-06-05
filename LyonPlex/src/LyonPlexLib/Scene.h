#pragma once

#include "SceneManager.h"

struct SceneEntity 
{
	Entity entity;
	std::string name;
};

class Scene
{
public:
	void Init();

	virtual void Start();
	virtual void Update();
	virtual void Release();

	void SetEcsManager(ECSManager* ecsManager) { mp_EcsManager = ecsManager; };

	void AddEntityToScene(Entity entity, const std::string& entityName); // Only entities who need to get update like player or camera, not background or trees ..
	
	SceneEntity CreateEntity(const std::string& entityName);

	Entity* GetEntity(const std::string& entityName);

	// Component Entity
	template<typename T>
	void AddComponent(const std::string& entityName, T* comp)
	{
		for (auto& entity : m_sceneEntities)
		{
			if (entity.name == entityName)
			{
				mp_EcsManager->AddComponent<T>(entity.entity, comp);
			}
		}
	}
	template<typename T>
	T* GetComponent(const std::string& entityName) const
	{
		for (auto& entity : m_sceneEntities)
		{
			if (entity.name == entityName)
			{
				return mp_EcsManager->GetComponent<T>(entity.entity);
			}
		}
	}

protected:

	ECSManager* mp_EcsManager;

	std::vector<SceneEntity> m_sceneEntities;
};

