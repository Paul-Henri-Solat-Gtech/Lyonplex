#include "pch.h"
#include "SceneManager.h"

void SceneManager::Init()
{
}

void SceneManager::StartScene()
{
	m_scene.Start();
}

void SceneManager::UpdateScene()
{
	m_scene.Update();
}

void SceneManager::ReleaseScene()
{
	m_scene.Release();
}
