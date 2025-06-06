﻿#include "pch.h"
#include "main.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) 
{
    GameManager game(hInstance);
    if (!game.Init())
        return -1;
    return game.Run();
}