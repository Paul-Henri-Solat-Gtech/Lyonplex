#include "pch.h"
#include "SampleScene2.h"

void SampleScene2::Start()
{
    OutputDebugStringA("\nHI ! I am sample scene. \n");

    //Entity eTriangle = mp_EcsManager->CreateEntity();
    //mp_EcsManager->AddComponent<MeshComponent>(eTriangle, new MeshComponent(0, 0));
    //mp_EcsManager->AddComponent<TransformComponent>(eTriangle, new TransformComponent());
    //mp_EcsManager->GetComponent<TransformComponent>(eTriangle)->position = { 0.60, 0.4, 0.2 };
    //mp_EcsManager->GetComponent<TransformComponent>(eTriangle)->scale = { 3, 3, 1 };
    //AddEntityToScene(eTriangle, "triangle");

    CreateEntity("camera");
    AddComponent<CameraComponent>("camera", new CameraComponent());
    GetComponent<TransformComponent>("camera")->position = { 0, 0, -1 };

    CreateEntity("cube");
    AddComponent<MeshComponent>("cube", new MeshComponent(2, 0));
    GetComponent<TransformComponent>("cube")->position = { 0, 0, -1 };
    GetComponent<TransformComponent>("cube")->dirty = true;

    CreateEntity("cube2");
    AddComponent<MeshComponent>("cube2", new MeshComponent(2, 0));
    GetComponent<TransformComponent>("cube2")->position = { 5, -0, -1 };
    GetComponent<TransformComponent>("cube2")->dirty = true;

    CreateEntity("cube3");
    AddComponent<MeshComponent>("cube3", new MeshComponent(2, 0));
    GetComponent<TransformComponent>("cube3")->position = { -5, -0, 0 };
    GetComponent<TransformComponent>("cube3")->dirty = true;

    CreateEntity("cube4");
    AddComponent<MeshComponent>("cube4", new MeshComponent(2, 0));
    GetComponent<TransformComponent>("cube4")->position = { 0, -0, 5 };
    GetComponent<TransformComponent>("cube4")->dirty = true;

    GetComponent<TransformComponent>("camera")->parent = { GetEntity("cube")->id };
    GetComponent<TransformComponent>("camera")->dirty = true;
}

void SampleScene2::Update(float deltatime)
{
    //Input
    if (InputManager::GetKeyIsPressed('Z'))
    {
        //OutputDebugStringA("\nZ is pressed ! \n");
        GetComponent<TransformComponent>("cube")->position.z += 1.f * deltatime;
        GetComponent<TransformComponent>("cube")->dirty = true;
        GetComponent<TransformComponent>("camera")->dirty = true;
    }
    if (InputManager::GetKeyIsPressed('S'))
    {
        GetComponent<TransformComponent>("cube")->position.z -= 1.f * deltatime;
        GetComponent<TransformComponent>("cube")->dirty = true;
        GetComponent<TransformComponent>("camera")->dirty = true;
    }
    if (InputManager::GetKeyIsPressed('Q'))
    {
        GetComponent<TransformComponent>("cube")->position.x -= 1.f * deltatime;
        GetComponent<TransformComponent>("cube")->dirty = true;
        GetComponent<TransformComponent>("camera")->dirty = true;
    }
    if (InputManager::GetKeyIsPressed('D'))
    {
        GetComponent<TransformComponent>("cube")->position.x += 1.f * deltatime;
        GetComponent<TransformComponent>("cube")->dirty = true;
        GetComponent<TransformComponent>("camera")->dirty = true;
    }
    if (InputManager::GetKeyIsPressed(VK_SPACE))
    {
        GetComponent<TransformComponent>("cube")->position.y += 1.f * deltatime;
        GetComponent<TransformComponent>("cube")->dirty = true;
        GetComponent<TransformComponent>("camera")->dirty = true;
    }
    if (InputManager::GetKeyIsPressed(VK_CONTROL))
    {
        GetComponent<TransformComponent>("cube")->position.y -= 1.f * deltatime;
        GetComponent<TransformComponent>("cube")->dirty = true;
        GetComponent<TransformComponent>("camera")->dirty = true;
        
    }
    if (InputManager::GetKeyIsReleased('A'))
    {
        ChangeScene("GameScene");
    }
}

void SampleScene2::Release()
{

}

