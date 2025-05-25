#include "pch.h"
#include "GameManager.h"

GameManager::GameManager(HINSTANCE hInstance) : m_hInstance(hInstance)
{

}

GameManager::~GameManager() 
{
    // Nettoyage si besoin
}

bool GameManager::Init() 
{
    // 1) Creer la fenetre
    if (!m_window.Init(m_hInstance, L"MonJeuDX12", 1280, 720))
        return false;

    // 2) Configurer le renderer avec le handle de la fenetre
    HWND hwnd = m_window.GetWindowHandle();
    m_renderer.SetWindowHandle(hwnd);
    m_renderer.Init();

    m_isRunning = true;
    return true;
}

int GameManager::Run() 
{
    // Message et boucle de rendu
    MSG msg = {};
    while (m_isRunning) {
        // Traiter tous les messages en attente
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                m_isRunning = false;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Enregistrement et envoi des commandes
        m_renderer.RecordCommands();
        m_renderer.ExecuteCommands();
        m_renderer.Present();

        // Synchronisation CPU/GPU
        m_renderer.SignalAndWait();
    }

    return static_cast<int>(msg.wParam);
}

void GameManager::ProcessMessage()
{
}

// helper pour la synchronisation
void GameManager::WaitForPreviousFrame() 
{
    m_renderer.SignalAndWait();
}