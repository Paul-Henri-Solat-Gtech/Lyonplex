#include "pch.h"
#include "Scene.h"

void Scene::Init()
{
}

void Scene::Start()
{
	OutputDebugStringA("\nScene has started ! \n");

	/*Entity eTriangle = m_ECS->CreateEntity();
	m_ECS->AddComponent<MeshComponent>(eTriangle, new MeshComponent(1, 0));

	Entity eSquare = m_ECS->CreateEntity();
	m_ECS->AddComponent<MeshComponent>(eSquare, new MeshComponent(0, 0));

	Entity eCube = m_ECS->CreateEntity();
	m_ECS->AddComponent<MeshComponent>(eCube, new MeshComponent(2, 0));*/
}

void Scene::Update()
{

}

void Scene::Release()
{
	m_sceneEntities.clear();
}

void Scene::AddEntityToScene(Entity entity, const std::string& entityName)
{
	SceneEntity newEntity;
	newEntity.entity = entity;
	newEntity.name = entityName;

	m_sceneEntities.push_back(newEntity);
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
