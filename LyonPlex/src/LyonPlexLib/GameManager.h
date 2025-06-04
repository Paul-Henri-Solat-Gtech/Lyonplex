#pragma once

#include "WindowPlex.h"
#include "RenderingManager.h"
#include "ECSManager.h"

#include "SceneManager.h"

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

private:
    void ProcessMessage(); // Gestion messages windows

    HINSTANCE m_hInstance;
    WindowPlex m_window;
    RenderingManager m_renderer;
    ECSManager m_ECS;
    bool m_isRunning = false;

    //scene
    SceneManager m_sceneManager;
};

