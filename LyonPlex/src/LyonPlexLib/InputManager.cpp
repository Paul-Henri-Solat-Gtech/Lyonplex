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
