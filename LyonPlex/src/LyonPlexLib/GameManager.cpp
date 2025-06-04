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

    m_ECS.Init(m_renderer.GetGraphicsDevice(), m_renderer.GetCommandManager(), m_renderer.GetRender3D());

    m_isRunning = true;
    return true;
}

int GameManager::Run() 
{
    Entity camEntity = m_ECS.CreateEntity();
    m_ECS.AddComponent<CameraComponent>(camEntity, new CameraComponent());


    Entity eSquare = m_ECS.CreateEntity();
    m_ECS.AddComponent<MeshComponent>(eSquare, new MeshComponent(1, 0));
    m_ECS.AddComponent<TransformComponent>(eSquare, new TransformComponent());
    m_ECS.GetComponent<TransformComponent>(eSquare)->position = { -0.60, 0.4, 0.2 };
    m_ECS.GetComponent<TransformComponent>(eSquare)->scale = { 2, 2, 1 };

    /*Entity eSquare2 = m_ECS.CreateEntity();
    m_ECS.AddComponent<MeshComponent>(eSquare2, new MeshComponent(1, 0));
    m_ECS.AddComponent<TransformComponent>(eSquare2, new TransformComponent());
    m_ECS.GetComponent<TransformComponent>(eSquare2)->position = { -0.20, 0.4, 0.2 };*/
    
    Entity eTriangle = m_ECS.CreateEntity();
    m_ECS.AddComponent<MeshComponent>(eTriangle, new MeshComponent(0, 0));
    m_ECS.AddComponent<TransformComponent>(eTriangle, new TransformComponent());
    m_ECS.GetComponent<TransformComponent>(eTriangle)->position = { 0.60, 0.4, 0.2 };
    m_ECS.GetComponent<TransformComponent>(eTriangle)->scale = { 3, 3, 1 };
    /*Entity eTriangle2 = m_ECS.CreateEntity();
    m_ECS.AddComponent<MeshComponent>(eTriangle2, new MeshComponent(0, 0));
    m_ECS.AddComponent<TransformComponent>(eTriangle2, new TransformComponent());
    m_ECS.GetComponent<TransformComponent>(eTriangle2)->position = { -0.20, -0.4, 0.2 };*/

    Entity eCube = m_ECS.CreateEntity();
    m_ECS.AddComponent<MeshComponent>(eCube, new MeshComponent(2, 0));
    m_ECS.AddComponent<TransformComponent>(eCube, new TransformComponent());
    m_ECS.GetComponent<TransformComponent>(eCube)->position = { 0.2, -0.6, 0 };
    
    /*Entity eCube2 = m_ECS.CreateEntity();
    m_ECS.AddComponent<MeshComponent>(eCube2, new MeshComponent(2, 0));
    m_ECS.AddComponent<TransformComponent>(eCube2, new TransformComponent());
    m_ECS.GetComponent<TransformComponent>(eCube2)->position = { -0.4, 0, 0 };*/
   /* m_ECS.GetComponent<TransformComponent>(eSquare)->position = { 0, 0.5, 2 };*/ // IL FAUT FAIRE UNE FONTION QUI APPEL TJRS dirty = true APRES CHAQUE MODIFICATION, SINON LE TRANSFORM EST PAS UPDATE

    int i = 0;
    // Message et boucle de rendu
    MSG msg = {};
    while (m_isRunning) {
        
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

        // Enregistrement et envoi des commandes
        
        // 3) Enregistrement des commandes de rendu dans la CommandList
        m_renderer.RecordCommands();
        
        // 4) Soumission des commandes au GPU pour execution
        m_renderer.ExecuteCommands();
        
        // 5) Presentation du back buffer a l’ecran (swap buffers)
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