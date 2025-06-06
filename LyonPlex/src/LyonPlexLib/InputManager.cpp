#include "pch.h"
#include "InputManager.h"

bool InputManager::GetKeyIsPressed(char keyName)
{
	if (GetAsyncKeyState(keyName) & 0x8000)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool InputManager::GetKeyIsReleased(char keyName)
{
	// tableau statique pour mémoriser l'état précédent de chaque touche
	static bool prevState[256] = {};

	bool isDownNow = (GetAsyncKeyState(keyName) & 0x8000) != 0;
	bool wasDown = prevState[(unsigned char)keyName];
	bool justReleased = (wasDown && !isDownNow);

	prevState[(unsigned char)keyName] = isDownNow;
	return justReleased;
}
