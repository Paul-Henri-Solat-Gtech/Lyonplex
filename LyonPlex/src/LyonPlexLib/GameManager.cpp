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
    if (!m_window.Init(m_hInstance, L"MonJeuDX12", 800, 600))
        return false;

    // 2) Configurer le renderer avec le handle de la fenetre
    HWND hwnd = m_window.GetWindowHandle();
    m_renderer.SetWindowHandle(hwnd);
    m_renderer.Init(&m_ECS); // A VOIR MODIFIER ET METTRE HWND COMME ARGUMENT EN POINTEUR (et mettre le init en bool)

    m_isRunning = true;
    return true;
}

int GameManager::Run() 
{
    // Message et boucle de rendu
    MSG msg = {};
    while (m_isRunning) {
        
        // 1) Gestion des messages Windows
        ProcessMessage();

        // 2) Traitement manuel des messages restants
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) 
            {
                // Si on reçoit WM_QUIT, on sort de la boucle
                m_isRunning = false;
            }
            TranslateMessage(&msg); // Prépare le message (gestion du clavier, etc.)
            DispatchMessage(&msg); // Appelle la WindowProcedure correspondante
        }

        // Enregistrement et envoi des commandes
        
        // 3) Enregistrement des commandes de rendu dans la CommandList
        m_renderer.RecordCommands();
        
        // 4) Soumission des commandes au GPU pour exécution
        m_renderer.ExecuteCommands();
        
        // 5) Présentation du back buffer à l’écran (swap buffers)
        m_renderer.Present();

        // Synchronisation CPU/GPU (on attend que le GPU ait fini)
        m_renderer.SynchroGPUCPU();
    }

    Release();

    return static_cast<int>(msg.wParam);
}

void GameManager::Release()
{
    m_renderer.Release();
}


void GameManager::ProcessMessage()
{

}