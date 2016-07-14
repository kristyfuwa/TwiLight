
#include "SystemClass.h"
SystemClass::SystemClass()
{
	bexit = false;
	m_pGraphics = nullptr;
	m_pInput = nullptr;
}

SystemClass::SystemClass(const SystemClass&)
{

}

SystemClass::~SystemClass()
{

}

bool SystemClass::Initialize()
{
	int screenWidth = 0;
	int screenHeight = 0;

	InitializeWindow(screenWidth, screenHeight);

	m_pInput = new Input();
	if (!m_pInput)
	{
		return false;
	}
	m_pInput->Initialize();


	m_pGraphics = new Graphics();
	if (m_pGraphics == nullptr)
	{
		return false;
	}
	bool result = m_pGraphics->Initilize(screenWidth, screenHeight, m_hWnd);
	if (!result)
	{
		return false;
	}
	return true;
}

void SystemClass::Shutdown()
{
	if (m_pGraphics)
	{
		m_pGraphics->ShutDown();
		delete m_pGraphics;
		m_pGraphics = nullptr;

	}
	if (m_pInput)
	{
		delete m_pInput;
		m_pInput = nullptr;
	}
	ShutdownWindows();
}

void SystemClass::Run()
{
	MSG msg;
	bool result = 1;
	bool done;
	ZeroMemory(&msg, sizeof(MSG));
	done = false;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = bexit;
			//render;

			//....
			//...
			Frame();
			if (result)
			{
				done = true;
			}

		}
	}
	return;

}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_KEYDOWN:
	{
		m_pInput->KeyDown((unsigned int)wparam);
		return 0;
	}
	break;
	case WM_KEYUP:
	{
		m_pInput->KeyUp((unsigned int)wparam);
		return 0;
	}
	break;
	default:
	{
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
	break;
	}
}

bool SystemClass::Frame()
{
	bool result;
	if (m_pInput->IsKeyDown(VK_ESCAPE))
	{
		return false;
	}
	result = m_pGraphics->Frame();
	if (!result)
	{
		return false;
	}
	return true;
}

void SystemClass::InitializeWindow(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	ApplicationHandle = this;
	//GetApplicationHandler
	m_hInstance = GetModuleHandle(NULL);
	//ApplicationName;
	m_applicationName = L"TwiLightEngine";
	//SetWindowClassParam;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndPro;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);//default black background;
	wc.lpszClassName = m_applicationName;
	wc.lpszMenuName = NULL;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	//GetScreenWidth/ScreenHeight;
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (Full_Screen)
	{
		//FullMode;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//Temp set device fullmode ,set default on exit;
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
		posX = posY = 0;
	}
	else
	{
		//Window mode;1024*768;
		screenWidth = 800;
		screenHeight = 600;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	if (Full_Screen)
	{
		m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
			posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);
	}
	else
	{
		m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
			WS_OVERLAPPEDWINDOW,
			posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);
	}


	//show window and set focu window;
	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);

	//hide mouse;
	//ShowCursor(false);
	return;
}

void SystemClass::ShutdownWindows()
{
	//Show Cursor;
	//ShowCusor(true);

	//Set Default ShowSetting;
	if (Full_Screen)
	{
		ChangeDisplaySettings(NULL, 0);
	}
	//Release WindowHinstance;
	DestroyWindow(m_hWnd);
	m_hWnd = nullptr;
	ApplicationHandle = nullptr;
	//Release Application;
	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = nullptr;

	return;
}

static LRESULT CALLBACK WndPro(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	break;
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}
	break;
	//MessageHandle other message;
	//MessageHandler(hwnd, umessage, wparam, lparam);
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	break;
	}
}