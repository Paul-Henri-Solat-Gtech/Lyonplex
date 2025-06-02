#pragma once

#include "Scene.h"

class SceneManager
{
public:
	void Init();

	void StartScene();
	void UpdateScene();
	void ReleaseScene();

	void SetScene(Scene newScene) { m_scene = newScene; };
	
	Scene GetScene() { return m_scene; };

private:

	// Scene active
	Scene m_scene;
	
	// Scene list
	std::vector<Scene*> m_scenes;
};

