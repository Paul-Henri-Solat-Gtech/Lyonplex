#include "pch.h"
#include "SceneManager.h"

void SceneManager::Init(ECSManager* ecsManager)
{
	mp_ecsManager = ecsManager;
}

void SceneManager::StartScene()
{
	if (m_scene.scene) 
	{
		m_scene.scene->Start();
		const char* sceneName = m_scene.sceneName.c_str();
		std::string newMsg = "\n" + std::string(sceneName) + " Has been Loaded !\n";
		OutputDebugStringA(newMsg.c_str());
	}
	else
	{
		OutputDebugStringA("\n-----------------------------------] \n No scene have been started :.( \n-----------------------------------] \n \n");
	}

}

void SceneManager::UpdateScene()
{
	if (m_scene.scene)
	{
		m_scene.scene->Update();
	}
}

void SceneManager::ReleaseScene()
{
	if (m_scene.scene)
	{
		m_scene.scene->Release();
	}
}

void SceneManager::CreateScene(Scene* scene, std::string sceneName)
{
	CreatedScene newScene;
	newScene.scene = scene;
	newScene.sceneName = sceneName;

	newScene.scene->SetEcsManager(mp_ecsManager);

	m_scenes.push_back(newScene);

	std::string newMsg = "\n[ " + newScene.sceneName + " Has been Created & Added ! ]";
	OutputDebugStringA(newMsg.c_str());
}

void SceneManager::SetScene(std::string sceneName)
{
	for (auto& createdScene : m_scenes) 
	{
		if (sceneName == createdScene.sceneName)
		{
			m_scene = createdScene;
		}
	}
}
