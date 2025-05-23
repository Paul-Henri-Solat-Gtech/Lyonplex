#pragma once

class WindowPlex
{
public:
	//Default value can be changed (also they always have to be at the end)
	bool Init(HINSTANCE appInstance, const std::wstring& windowName = L"MyNewWindow", int windowWidth = 1920, int windowHeight = 1080);
		
	HWND GetWindowHandle() const { return m_windowHandle; };

private:
	static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

	HWND m_windowHandle = nullptr;
};

