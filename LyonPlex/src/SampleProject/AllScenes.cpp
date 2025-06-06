#include "pch.h"
#include "AllScenes.h"

// Includes scenes here :
#include "SampleScene2.h"
#include "GameScene.h"

// ..

void AllScenes::Init(GameManager* gameManager)
{
    mp_gameManager = gameManager;
    CreateAllScenes();

    // START SCENE
    //StartScene("SampleScene2");
    StartScene("GameScene");
}

void AllScenes::CreateAllScenes()
{
    // Create new scenes
    SampleScene2* newSampleScene2 = new SampleScene2();
    mp_gameManager->GetSceneManager()->CreateScene(newSampleScene2, "SampleScene2");
    GameScene* newGameScene = new GameScene();
    mp_gameManager->GetSceneManager()->CreateScene(newGameScene, "GameScene");
}

void AllScenes::StartScene(std::string sceneName)
{
    // Set & Start scene
    mp_gameManager->GetSceneManager()->SetScene(sceneName);
}
