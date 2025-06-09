#pragma once

#include "Scene.h"

class GameManager;

struct CreatedScene 
{
	std::string sceneName;
	Scene* scene;
};

class SceneManager
{
public:
	void Init(ECSManager* ecsManager,GameManager* gameManager , HWND windowHandle);

	void StartScene();
	void UpdateScene(float deltatime);
	void ReleaseScene();

	void CreateScene(Scene* scene, std::string sceneName);

	void SetScene(std::string sceneName);
	
	//& GetScene() { return *m_scene; };

	GameManager* GetGameManager() { return mp_gameManager; };

private:
	// Ressources
	ECSManager* mp_ecsManager;
	GameManager* mp_gameManager;
	HWND m_windowHandle; // Just for debug and window name

	// Scene active
	CreatedScene m_scene;
	bool m_sceneAsSarted = true;

	// Scene list
	std::vector<CreatedScene> m_scenes;
};

