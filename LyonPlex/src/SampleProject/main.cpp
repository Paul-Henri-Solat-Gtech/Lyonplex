#include "pch.h"
#include "main.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) 
{
    GameManager game(hInstance);
    if (!game.Init())
        return -1;

    // Create Scenes (Go to this class to add scenes & set first scene)
    AllScenes allScenes;
    allScenes.Init(&game);

    return game.Run();
}