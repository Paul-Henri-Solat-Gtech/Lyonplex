#pragma once

#include "WindowPlex.h"
#include "RenderingManager.h"
#include "ECSManager.h"

#include "SceneManager.h"

#include "chrono"

class GameManager 
{
public:
    GameManager(HINSTANCE hInstance);
    ~GameManager();

    // Initialisation Window with directx
    bool Init();

    // Boucle principale
    int Run();

    void Release();

    SceneManager* GetSceneManager() { return &m_sceneManager; };
    float& GetDeltaTime() { return m_deltaTime; };

private:
    void ProcessMessage(); // Gestion messages windows

    HINSTANCE m_hInstance;
    WindowPlex m_window;
    RenderingManager m_renderer;
    ECSManager m_ECS;
    bool m_isRunning = false;

    //deltatime
    float m_deltaTime;

    //scene
    SceneManager m_sceneManager;
};

