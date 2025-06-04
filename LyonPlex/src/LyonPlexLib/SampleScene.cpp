#include "pch.h"
#include "SampleScene.h"

void SampleScene::Start()
{
	OutputDebugStringA("\nHI ! I am sample scene. \n");
    
   Entity camEntity = mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<CameraComponent>(camEntity, new CameraComponent());


    Entity eSquare = mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<MeshComponent>(eSquare, new MeshComponent(1, 0));
   mp_EcsManager->AddComponent<TransformComponent>(eSquare, new TransformComponent());
   mp_EcsManager->GetComponent<TransformComponent>(eSquare)->position = { -0.60, 0.4, 0.2 };
   mp_EcsManager->GetComponent<TransformComponent>(eSquare)->scale = { 2, 2, 1 };

    /*Entity eSquare2 =mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<MeshComponent>(eSquare2, new MeshComponent(1, 0));
   mp_EcsManager->AddComponent<TransformComponent>(eSquare2, new TransformComponent());
   mp_EcsManager->GetComponent<TransformComponent>(eSquare2)->position = { -0.20, 0.4, 0.2 };*/

    Entity eTriangle = mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<MeshComponent>(eTriangle, new MeshComponent(0, 0));
   mp_EcsManager->AddComponent<TransformComponent>(eTriangle, new TransformComponent());
   mp_EcsManager->GetComponent<TransformComponent>(eTriangle)->position = { 0.60, 0.4, 0.2 };
   mp_EcsManager->GetComponent<TransformComponent>(eTriangle)->scale = { 3, 3, 1 };
    /*Entity eTriangle2 =mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<MeshComponent>(eTriangle2, new MeshComponent(0, 0));
   mp_EcsManager->AddComponent<TransformComponent>(eTriangle2, new TransformComponent());
   mp_EcsManager->GetComponent<TransformComponent>(eTriangle2)->position = { -0.20, -0.4, 0.2 };*/

    Entity eCube = mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<MeshComponent>(eCube, new MeshComponent(2, 0));
   mp_EcsManager->AddComponent<TransformComponent>(eCube, new TransformComponent());
   mp_EcsManager->GetComponent<TransformComponent>(eCube)->position = { 0.2, -0.6, 0 };

    /*Entity eCube2 =mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<MeshComponent>(eCube2, new MeshComponent(2, 0));
   mp_EcsManager->AddComponent<TransformComponent>(eCube2, new TransformComponent());
   mp_EcsManager->GetComponent<TransformComponent>(eCube2)->position = { -0.4, 0, 0 };*/
    /*mp_EcsManager->GetComponent<TransformComponent>(eSquare)->position = { 0, 0.5, 2 };*/ // IL FAUT FAIRE UNE FONTION QUI APPEL TJRS dirty = true APRES CHAQUE MODIFICATION, SINON LE TRANSFORM EST PAS UPDATE

	/*Entity eTriangle = mp_EcsManager->CreateEntity();
	mp_EcsManager->AddComponent<MeshComponent>(eTriangle, new MeshComponent(0, 0));

	Entity eSquare = mp_EcsManager->CreateEntity();
	mp_EcsManager->AddComponent<MeshComponent>(eSquare, new MeshComponent(1, 0));

	Entity eCube = mp_EcsManager->CreateEntity();
	mp_EcsManager->AddComponent<MeshComponent>(eCube, new MeshComponent(2, 0));*/
}

void SampleScene::Update()
{
}

void SampleScene::Release()
{
}
