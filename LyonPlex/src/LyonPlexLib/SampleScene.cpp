#include "pch.h"
#include "SampleScene.h"

void SampleScene::Start()
{
	OutputDebugStringA("\nHI ! I am sample scene. \n");
    
   Entity camEntity = mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<CameraComponent>(camEntity, new CameraComponent());
   //mp_EcsManager->AddComponent<TransformComponent>(camEntity, new TransformComponent());

   AddEntityToScene(camEntity, "camera");

   Entity eSquare = mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<MeshComponent>(eSquare, new MeshComponent(1, 0));
   mp_EcsManager->AddComponent<TransformComponent>(eSquare, new TransformComponent());
   mp_EcsManager->GetComponent<TransformComponent>(eSquare)->position = { -0.60, 0.4, 0.2 };
   //mp_EcsManager->GetComponent<TransformComponent>(eSquare)->position.x += 0.9f;
   mp_EcsManager->GetComponent<TransformComponent>(eSquare)->scale = { 2, 2, 1 };
   AddEntityToScene(eSquare, "square");

   Entity eTriangle = mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<MeshComponent>(eTriangle, new MeshComponent(0, 0));
   mp_EcsManager->AddComponent<TransformComponent>(eTriangle, new TransformComponent());
   mp_EcsManager->GetComponent<TransformComponent>(eTriangle)->position = { 0.60, 0.4, 0.2 };
   mp_EcsManager->GetComponent<TransformComponent>(eTriangle)->scale = { 3, 3, 1 };
   AddEntityToScene(eTriangle, "triangle");

   Entity eCube = mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<MeshComponent>(eCube, new MeshComponent(2, 0));
   mp_EcsManager->AddComponent<TransformComponent>(eCube, new TransformComponent());
   mp_EcsManager->GetComponent<TransformComponent>(eCube)->position = { 0.2, -0.6, 0 };
   AddEntityToScene(eCube, "cube");

    /*Entity eCube2 =mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<MeshComponent>(eCube2, new MeshComponent(2, 0));
   mp_EcsManager->AddComponent<TransformComponent>(eCube2, new TransformComponent());
   mp_EcsManager->GetComponent<TransformComponent>(eCube2)->position = { -0.4, 0, 0 };*/
    /*mp_EcsManager->GetComponent<TransformComponent>(eSquare)->position = { 0, 0.5, 2 };*/ // IL FAUT FAIRE UNE FONTION QUI APPEL TJRS dirty = true APRES CHAQUE MODIFICATION, SINON LE TRANSFORM EST PAS UPDATE
}

void SampleScene::Update()
{
    //Input
    if (InputManager::GetKeyIsPressed('Z'))
    {
        //OutputDebugStringA("\nZ is pressed ! \n");
        mp_EcsManager->GetComponent<TransformComponent>(*GetEntity("cube"))->position.z += 0.01f;
    }
    if (InputManager::GetKeyIsPressed('S'))
    {
        //OutputDebugStringA("\nZ is pressed ! \n");
        mp_EcsManager->GetComponent<TransformComponent>(*GetEntity("cube"))->position.z -= 0.01f;
    }
    if (InputManager::GetKeyIsPressed('Q'))
    {
        //OutputDebugStringA("\nZ is pressed ! \n");
        mp_EcsManager->GetComponent<TransformComponent>(*GetEntity("cube"))->position.x -= 0.01f;
    }
    if (InputManager::GetKeyIsPressed('D'))
    {
        //OutputDebugStringA("\nZ is pressed ! \n");
        mp_EcsManager->GetComponent<TransformComponent>(*GetEntity("cube"))->position.x += 0.01f;
    }
}

void SampleScene::Release()
{

}
