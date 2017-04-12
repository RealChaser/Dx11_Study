#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

class Graphics;
class Input;
class System
{
public:
	System();
	System(const System&);
	~System();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	Input* m_pInput;
	Graphics* m_pGraphics;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static System* g_pApplicationHandle = 0;