#pragma once

#include "Scene.h"

struct CreatedScene 
{
	std::string sceneName;
	Scene* scene;
};

class SceneManager
{
public:
	void Init(ECSManager* ecsManager);

	void StartScene();
	void UpdateScene();
	void ReleaseScene();

	void CreateScene(Scene* scene, std::string sceneName);

	void SetScene(std::string sceneName);
	
	//& GetScene() { return *m_scene; };

private:
	// Ressources
	ECSManager* mp_ecsManager;

	// Scene active
	CreatedScene m_scene;
	
	// Scene list
	std::vector<CreatedScene> m_scenes;
};

