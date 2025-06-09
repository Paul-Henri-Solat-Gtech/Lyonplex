#include "pch.h"
#include "SceneManager.h"
#include "GameManager.h"

void SceneManager::Init(ECSManager* ecsManager, GameManager* gameManager, HWND windowHandle)
{
	mp_ecsManager = ecsManager;
	mp_gameManager = gameManager;
	m_windowHandle = windowHandle;
	m_sceneAsSarted = true;
}

void SceneManager::StartScene()
{
	if (m_scene.scene)
	{
		m_scene.scene->Init(this);
		const char* sceneName = m_scene.sceneName.c_str();
		std::string newMsg = "\n" + std::string(sceneName) + " Has been Loaded !\n";
		OutputDebugStringA(newMsg.c_str());
	}
	else
	{
		OutputDebugStringA("\n-----------------------------------] \n No scene have been started :.( \n-----------------------------------] \n \n");
	}

}

void SceneManager::UpdateScene(float deltatime)
{
	if (m_scene.scene)
	{
		m_scene.scene->Update(deltatime);
	}
}

void SceneManager::ReleaseScene()
{
	if (m_scene.scene)
	{
		m_scene.scene->Release();
		//m_sceneAsSarted = false;
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
			if (m_scene.scene)
			{
				ReleaseScene();		// Clear actual scene
			}

			m_scene = createdScene; // Set the new scene
			StartScene();			// Start the new scene

			// Update WindowTitle
			std::string titleW = "[LyonPlex] [ scene : " + sceneName + " ]";
			std::wstring wname(titleW.begin(), titleW.end());
			SetWindowText(m_windowHandle, wname.c_str());
		}
	}
}
