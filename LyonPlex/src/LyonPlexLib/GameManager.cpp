#include "pch.h"
#include "GameManager.h"
#include "Utils.h"

// Scenes
#include "SampleScene.h"

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

    m_ECS.Init(m_renderer.GetGraphicsDevice(), m_renderer.GetCommandManager(), m_renderer.GetRender3D());

    m_isRunning = true;

    // 3) Init sceneManager
    m_sceneManager.Init(&m_ECS,this,hwnd);
    
    // 4) Create new scene (OPTIONAL IN LIB)
    SampleScene* newSampleScene = new SampleScene();
    m_sceneManager.CreateScene(newSampleScene, "SampleScene");

    return true;
}

int GameManager::Run() 
{
   
    int i = 0;
    // Message et boucle de rendu
    MSG msg = {};

    // Delatime
    float t = Utils::getTimeSeconds();

    while (m_isRunning) 
    {
        // Delatime
        m_deltaTime = Utils::getTimeSeconds() - t;
        t = Utils::getTimeSeconds();


        // 1) Gestion des messages Windows
        ProcessMessage();
        float j = i * 0.1;
        //m_ECS.GetComponent<TransformComponent>(eCube)->position = { 0, 0, j };
        i++;
        // 2) Traitement manuel des messages restants
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) 
            {
                // Si on reçoit WM_QUIT, on sort de la boucle
                m_isRunning = false;
            }
            TranslateMessage(&msg); // Prepare le message (gestion du clavier, etc.)
            DispatchMessage(&msg); // Appelle la WindowProcedure correspondante
        }
        // UPDATE
        m_renderer.Update();
        m_ECS.m_systemMgr.UpdateAll(0);
        m_sceneManager.UpdateScene(m_deltaTime);

        // Enregistrement et envoi des commandes
        
        // 3) Enregistrement des commandes de rendu dans la CommandList
        m_renderer.RecordCommands();
        
        // 4) Soumission des commandes au GPU pour execution
        m_renderer.ExecuteCommands();
        
        // 5) Presentation du back buffer a l’ecran (swap buffers)
        m_renderer.Present();

        // Synchronisation CPU/GPU (on attend que le GPU ait fini)
        m_renderer.SynchroGPUCPU();

        m_ECS.EndFrame();
    }

    Release();

    return static_cast<int>(msg.wParam);
}

void GameManager::Release()
{
    m_renderer.Release();
    m_sceneManager.ReleaseScene();
}


void GameManager::ProcessMessage()
{

}