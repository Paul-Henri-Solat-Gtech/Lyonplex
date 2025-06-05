#pragma once

class WindowPlex
{
public:
	//Default value can be changed (also they always have to be at the end)
	bool Init(HINSTANCE appInstance, const std::wstring& windowName = L"MyNewWindow", float windowWidth = 1920, float windowHeight = 1080);
		
	HWND GetWindowHandle() const { return m_windowHandle; };

	float GetWindowWidth() { return m_winWidth; }
	float GetWindowHeight() { return m_winHeight; }

private:
	static LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);

	HWND m_windowHandle = nullptr;

	float m_winWidth, m_winHeight;
};

