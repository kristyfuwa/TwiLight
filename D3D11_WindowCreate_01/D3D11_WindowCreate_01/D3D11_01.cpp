#include<windows.h>
//WindowClassName;
LPCWSTR  m_applicationName;

//ApplicationProgramHandle;
HINSTANCE m_hinstance;

//WindowHandle;
HWND m_hwnd;

bool bexit = false;

//Is Full Screen;
const bool FULL_SCREEN = false;

LRESULT CALLBACK WndPro(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);
//initializeWindowClass;
void InitializeWindows(int & screenWidth, int & screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	//GetApplicationHandler
	m_hinstance = GetModuleHandle(NULL);
	//ApplicationName;
	m_applicationName = L"TwiLightEngine";
	//SetWindowClassParam;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndPro;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
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

	if (FULL_SCREEN)
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
		screenWidth = 1024;
		screenHeight = 768;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	if (FULL_SCREEN)
	{
		m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
			posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);
	}
	else
	{
		m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
			WS_OVERLAPPEDWINDOW,
			posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);
	}


	//show window and set focu window;
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	//hide mouse;
	//ShowCursor(false);
	return;
}

//initializeWindowFunction;
bool Initialize()
{
	int screenWidth = 0, screenHeight = 0;
	//InitializeWindow;
	InitializeWindows(screenWidth, screenHeight);
	return true;
}

//Handle Message;
void Run()
{
	MSG msg;
	bool done = 0;
	bool result = 1;

	//initializeMsg;
	ZeroMemory(&msg, sizeof(MSG));

	//while handle message;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = bexit;//press ESC to exit;
			//Render;
			//...........
			//...........
			if (result)
			{
				done = true;
			}
		}
	}
	return;
}

//Close Window
void ShutdownWindows()
{
	//Show Cursor;
	//ShowCusor(true);

	//Set Default ShowSetting;
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}
	//Release WindowHinstance;
	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	//Release Application;
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = nullptr;

	return;
}

//WindowCallBackFuction;
static LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_KEYDOWN:
	{
		if (wparam == VK_ESCAPE)
			bexit = true;
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
LRESULT CALLBACK WndPro(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
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
		return MessageHandler(hwnd, umessage, wparam, lparam);
	}
		break;
	}
}




int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmd, int iCmdShow)
{
	Initialize();
	Run();
	ShutdownWindows();
	return 0;
}




