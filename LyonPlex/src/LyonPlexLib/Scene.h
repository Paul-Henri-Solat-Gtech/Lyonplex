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

	Entity* GetEntity(const std::string& entityName);

protected:

	ECSManager* mp_EcsManager;
	//std::string m_SceneName;

	std::vector<SceneEntity> m_sceneEntities;
};

