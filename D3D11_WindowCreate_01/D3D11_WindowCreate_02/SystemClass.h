#pragma once

#define WIN32_LEAN_AND_MEAN
#include<windows.h>
#include "Input.h"
#include "Graphics.h"

static bool bexit = false;

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&);
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
private:
	bool Frame();
	void InitializeWindow(int&, int&);
	void ShutdownWindows();

	LPCWSTR				m_applicationName;
	HINSTANCE			m_hInstance;
	HWND				m_hWnd;
	Graphics*			m_pGraphics;
	Input*				m_pInput;
};
static LRESULT CALLBACK WndPro(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
static SystemClass* ApplicationHandle = 0;