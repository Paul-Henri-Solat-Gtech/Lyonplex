#include "pch.h"
#include "SampleScene.h"

void SampleScene::Start()
{
	OutputDebugStringA("\nHI ! I am sample scene. \n");

	Entity eTriangle = mp_EcsManager->CreateEntity();
	mp_EcsManager->AddComponent<MeshComponent>(eTriangle, new MeshComponent(1, 0));

	Entity eSquare = mp_EcsManager->CreateEntity();
	mp_EcsManager->AddComponent<MeshComponent>(eSquare, new MeshComponent(0, 0));

	Entity eCube = mp_EcsManager->CreateEntity();
	mp_EcsManager->AddComponent<MeshComponent>(eCube, new MeshComponent(2, 0));
}

void SampleScene::Update()
{
}

void SampleScene::Release()
{
}
