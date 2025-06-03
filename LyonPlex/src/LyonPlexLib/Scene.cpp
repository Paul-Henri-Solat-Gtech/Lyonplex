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

}
