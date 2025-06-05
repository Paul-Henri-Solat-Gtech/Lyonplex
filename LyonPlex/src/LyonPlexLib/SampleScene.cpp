#include "pch.h"
#include "SampleScene.h"

void SampleScene::Start()
{
	OutputDebugStringA("\nHI ! I am sample scene. \n");
    
   Entity camEntity = mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<CameraComponent>(camEntity, new CameraComponent());
   mp_EcsManager->AddComponent<TransformComponent>(camEntity, new TransformComponent());
   mp_EcsManager->GetComponent<TransformComponent>(camEntity)->position = { 0, 0, -1 };

   AddEntityToScene(camEntity, "camera");

   //Entity eSquare = mp_EcsManager->CreateEntity();
   //mp_EcsManager->AddComponent<MeshComponent>(eSquare, new MeshComponent(1, 0));
   //mp_EcsManager->AddComponent<TransformComponent>(eSquare, new TransformComponent());
   //mp_EcsManager->GetComponent<TransformComponent>(eSquare)->position = { -0.60, 0.4, 0.2 };
   ////mp_EcsManager->GetComponent<TransformComponent>(eSquare)->position.x += 0.9f;
   //mp_EcsManager->GetComponent<TransformComponent>(eSquare)->scale = { 2, 2, 1 };
   //AddEntityToScene(eSquare, "square");

   //Entity eTriangle = mp_EcsManager->CreateEntity();
   //mp_EcsManager->AddComponent<MeshComponent>(eTriangle, new MeshComponent(0, 0));
   //mp_EcsManager->AddComponent<TransformComponent>(eTriangle, new TransformComponent());
   //mp_EcsManager->GetComponent<TransformComponent>(eTriangle)->position = { 0.60, 0.4, 0.2 };
   //mp_EcsManager->GetComponent<TransformComponent>(eTriangle)->scale = { 3, 3, 1 };
   //AddEntityToScene(eTriangle, "triangle");

   Entity eCube = mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<MeshComponent>(eCube, new MeshComponent(2, 0));
   mp_EcsManager->AddComponent<TransformComponent>(eCube, new TransformComponent());
   mp_EcsManager->GetComponent<TransformComponent>(eCube)->position = { 0.2, -0.6, 0 };
   mp_EcsManager->GetComponent<TransformComponent>(eCube)->dirty = true;
   AddEntityToScene(eCube, "cube");

   Entity eCube2 = mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<MeshComponent>(eCube2, new MeshComponent(2, 0));
   mp_EcsManager->AddComponent<TransformComponent>(eCube2, new TransformComponent());
   mp_EcsManager->GetComponent<TransformComponent>(eCube2)->position = { 5, -0, 0 };
   mp_EcsManager->GetComponent<TransformComponent>(eCube2)->dirty = true;
   AddEntityToScene(eCube2, "cube2");

   Entity eCube3 = mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<MeshComponent>(eCube3, new MeshComponent(2, 0));
   mp_EcsManager->AddComponent<TransformComponent>(eCube3, new TransformComponent());
   mp_EcsManager->GetComponent<TransformComponent>(eCube3)->position = { -5, -0, 0 };
   mp_EcsManager->GetComponent<TransformComponent>(eCube3)->dirty = true;
   AddEntityToScene(eCube3, "eCube3");
   Entity eCube4 = mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<MeshComponent>(eCube4, new MeshComponent(2, 0));
   mp_EcsManager->AddComponent<TransformComponent>(eCube4, new TransformComponent());
   mp_EcsManager->GetComponent<TransformComponent>(eCube4)->position = { 0, -0, 5 };
   mp_EcsManager->GetComponent<TransformComponent>(eCube4)->dirty = true;
   AddEntityToScene(eCube4, "eCube4");
   Entity eCube5 = mp_EcsManager->CreateEntity();
   mp_EcsManager->AddComponent<MeshComponent>(eCube5, new MeshComponent(2, 0));
   mp_EcsManager->AddComponent<TransformComponent>(eCube5, new TransformComponent());
   mp_EcsManager->GetComponent<TransformComponent>(eCube5)->position = { 0, -0, 5 };
   mp_EcsManager->GetComponent<TransformComponent>(eCube5)->dirty = true;
   AddEntityToScene(eCube5, "eCube5");

   mp_EcsManager->GetComponent<TransformComponent>(camEntity)->parent = { eCube.id };
   mp_EcsManager->GetComponent<TransformComponent>(camEntity)->dirty = true;

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
        mp_EcsManager->GetComponent<TransformComponent>(*GetEntity("cube"))->dirty = true;
        mp_EcsManager->GetComponent<TransformComponent>(*GetEntity("camera"))->dirty = true;
    }
    if (InputManager::GetKeyIsPressed('S'))
    {
        //OutputDebugStringA("\nZ is pressed ! \n");
        mp_EcsManager->GetComponent<TransformComponent>(*GetEntity("cube"))->position.z -= 0.01f;
        mp_EcsManager->GetComponent<TransformComponent>(*GetEntity("cube"))->dirty = true;
        mp_EcsManager->GetComponent<TransformComponent>(*GetEntity("camera"))->dirty = true;
    }
    if (InputManager::GetKeyIsPressed('Q'))
    {
        //OutputDebugStringA("\nZ is pressed ! \n");
        mp_EcsManager->GetComponent<TransformComponent>(*GetEntity("cube"))->position.x -= 0.01f;
        mp_EcsManager->GetComponent<TransformComponent>(*GetEntity("cube"))->dirty = true;
        mp_EcsManager->GetComponent<TransformComponent>(*GetEntity("camera"))->dirty = true;
    }
    if (InputManager::GetKeyIsPressed('D'))
    {
        //OutputDebugStringA("\nZ is pressed ! \n");
        mp_EcsManager->GetComponent<TransformComponent>(*GetEntity("cube"))->position.x += 0.01f;
        mp_EcsManager->GetComponent<TransformComponent>(*GetEntity("cube"))->dirty = true;
        mp_EcsManager->GetComponent<TransformComponent>(*GetEntity("camera"))->dirty = true;
    }
}

void SampleScene::Release()
{

}
