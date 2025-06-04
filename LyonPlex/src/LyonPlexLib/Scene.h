#pragma once

#include "SceneManager.h"

class Scene
{
public:
	void Init();

	virtual void Start();
	virtual void Update();
	virtual void Release();

	void SetEcsManager(ECSManager* ecsManager) { mp_EcsManager = ecsManager; };

protected:

	ECSManager* mp_EcsManager;
	//std::string m_SceneName;
};

