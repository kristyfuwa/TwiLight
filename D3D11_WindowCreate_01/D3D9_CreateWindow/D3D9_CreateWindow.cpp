//#include <windows.h>
#include <D3D9.h>
#pragma warning(disable:4996)
#include <strsafe.h>
#pragma warning(disable:4996)

#pragma comment(lib,"d3d9.lib")

LPDIRECT3D9			g_pD3D = nullptr;
LPDIRECT3DDEVICE9	g_pd3dDevice = nullptr;

HRESULT InitD3D(HWND hWnd)
{
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		return E_FAIL;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	return S_OK;
}

VOID Cleanup()
{
	if (g_pd3dDevice != nullptr)
		g_pd3dDevice->Release();
	if (g_pD3D != nullptr)
		g_pD3D->Release();
}

VOID Render()
{
	if (NULL == g_pd3dDevice)
		return;
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 255, 255), 1.0f, 0);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		g_pd3dDevice->EndScene();
	}

	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}

LRESULT CALLBACK MsgPro(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		Render();
		ValidateRect(hwnd, NULL);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hwnd, umessage, wparam, lparam);
}


INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	UNREFERENCED_PARAMETER(hInst);
	//告诉编译器，已经使用了该变量，不必检测警告！

	WNDCLASSEX wc = { sizeof(WNDCLASSEX),CS_CLASSDC,MsgPro,0L,0L,GetModuleHandle(NULL),NULL,NULL,NULL,NULL,L"TwiLight",NULL };

	RegisterClassEx(&wc);

	HWND hWnd = CreateWindowEx(WS_EX_APPWINDOW, L"TwiLight", L"TwiLight", WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, GetDesktopWindow(), NULL, wc.hInstance, NULL);

	if (SUCCEEDED(InitD3D(hWnd)))
	{
		ShowWindow(hWnd, SW_SHOWDEFAULT);
		UpdateWindow(hWnd);

		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);

			DispatchMessage(&msg);
		}
	}

	UnregisterClass(L"TwiLight", wc.hInstance);
	return 0;
}






