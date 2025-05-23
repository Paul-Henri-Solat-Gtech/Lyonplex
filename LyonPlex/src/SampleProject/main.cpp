#include "pch.h"
#include "main.h"

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int)
{
    WindowPlex window;
    if (!window.Init(hInstance, L"SampleProject",800,800))
        return -1;

    // Boucle de messages non-bloquante (pour y greffer ton rendu DirectX)
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // … DirectX logic here …
    }

    //Better than just "return 0";
    return static_cast<int>(msg.wParam);
}