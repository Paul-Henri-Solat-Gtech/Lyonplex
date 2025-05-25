#pragma once

#include "WindowPlex.h"
#include "RenderingManager.h"

class GameManager 
{
public:
    GameManager(HINSTANCE hInstance);
    ~GameManager();

    // Initialisation Window with directx
    bool Init();

    // Boucle principale
    int Run();

private:
    void ProcessMessage();
    void WaitForPreviousFrame();

    HINSTANCE m_hInstance;
    WindowPlex m_window;
    RenderingManager m_renderer;
    bool m_isRunning = false;
};

