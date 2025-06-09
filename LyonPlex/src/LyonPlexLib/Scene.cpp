#include "pch.h"
#include "Scene.h"

void Scene::Init(SceneManager* sceneManager)
{
	mp_sceneManager = sceneManager;
	Start();
}

void Scene::Start()
{
	OutputDebugStringA("\nScene has started ! \n");
}

void Scene::Update(float deltatime)
{

}

void Scene::Release()
{
	m_sceneEntities.clear();
	mp_EcsManager->ClearAllEntities();
}

void Scene::ChangeScene(std::string sceneName)
{
	mp_sceneManager->SetScene(sceneName);
}

void Scene::AddEntityToScene(Entity entity, const std::string& entityName)
{
	SceneEntity newEntity;
	newEntity.entity = entity;
	newEntity.name = entityName;

	m_sceneEntities.push_back(newEntity);
}

SceneEntity Scene::CreateEntity(const std::string& entityName)
{
	Entity newEntity = mp_EcsManager->CreateEntity();

	AddEntityToScene(newEntity, entityName);

	//	Adding basics component(s) for any entity in scene & default parameters :
	//	TRANSFORM
	AddComponent<TransformComponent>(entityName, new TransformComponent());
	GetComponent<TransformComponent>(entityName)->position = { 0, 0, 0 };
	GetComponent<TransformComponent>(entityName)->dirty = true;
	//	... Add other component here if needed

	return SceneEntity();
}

Entity* Scene::GetEntity(const std::string& entityName)
{
	for (auto& entity : m_sceneEntities)
	{
		if (entity.name == entityName) 
		{
			return &entity.entity;
		}
	}
}
