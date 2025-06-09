#include "pch.h"
#include "GameScene.h"

void GameScene::Start()
{
    CreateEntity("camera");
    AddComponent<CameraComponent>("camera", new CameraComponent());
    GetComponent<TransformComponent>("camera")->position = { 0, 0, -1 };

    CreateEntity("cube");
    AddComponent<MeshComponent>("cube", new MeshComponent(2, 0));
    GetComponent<TransformComponent>("cube")->position = { 0, 0, -1 };
    GetComponent<TransformComponent>("cube")->dirty = true;

    GetComponent<TransformComponent>("camera")->parent = { GetEntity("cube")->id };
    GetComponent<TransformComponent>("camera")->dirty = true;
}

void GameScene::Update(float deltatime)
{
    if (InputManager::GetKeyIsReleased('A'))
    {
        ChangeScene("SampleScene2");
    }
}

void GameScene::Release()
{
}
