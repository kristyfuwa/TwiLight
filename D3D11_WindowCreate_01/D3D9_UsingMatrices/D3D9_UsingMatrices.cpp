#include <D3D9.h>
#include <d3dx9math.h>
#include <mmsystem.h>
#pragma warning(disable:4996)
#include <strsafe.h>
#pragma warning(disable:4996)

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9d.lib")
#pragma comment(lib,"winmm.lib")
//d3d9.lib
//d3dx10d.lib
//d3dx9d.lib
//dxerr.lib
//dxguid.lib
//winmm.lib
//comctl32.lib

LPDIRECT3D9				g_pD3D = nullptr;
LPDIRECT3DDEVICE9		g_pd3dDevice = nullptr;
LPDIRECT3DVERTEXBUFFER9	g_pVB = nullptr;
struct CUSTOMVERTEX
{
	FLOAT	x, y, z;
	DWORD	color;
};

//RHW是D3D里面的一种顶点格式，是一种自带视图、投影、世界等变换和光线计算等处理的顶点格式，
//而且这些处理我们无法用SetTransform等进行的转换。

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

//FVF;flexible vertex format


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

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	return S_OK;
}

HRESULT InitGeometry()
{
	CUSTOMVERTEX vertices[] =
	{
		{ -1.0f,-1.0f, 0.0f, 0xffff0000, },
		{ 1.0f,-1.0f, 0.0f, 0xff0000ff, },
		{ 0.0f, 1.0f, 0.0f, 0xffffffff, },
	};
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(3 * sizeof(CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	VOID* pVertices;
	if (FAILED(g_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
	{
		return E_FAIL;
	}
	memcpy(pVertices, vertices, sizeof(vertices));

	g_pVB->Unlock();
	return S_OK;
}

VOID Cleanup()
{
	if (g_pVB != nullptr)
		g_pVB->Release();

	if (g_pd3dDevice != nullptr)
		g_pd3dDevice->Release();

	if (g_pD3D != nullptr)
		g_pD3D->Release();
}

VOID SetupMatrices()
{
	D3DXMATRIXA16 matWorld;
	UINT iTime = timeGetTime() % 1000;
	FLOAT fAngle = iTime*(2.0f*D3DX_PI) / 1000.0f;
	D3DXMatrixRotationY(&matWorld, fAngle);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	D3DXVECTOR3 vEyPt(0.0f, 3.0f, -5.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	D3DXMATRIXA16	matView;
	D3DXMatrixLookAtLH(&matView, &vEyPt, &vLookatPt, &vUpVec);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	D3DXMATRIXA16	matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4.0f, 1.0f, 1.0f, 100.0f);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);


}

VOID Render()
{
	if (NULL == g_pd3dDevice)
		return;
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 255, 255), 1.0f, 0);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		SetupMatrices();

		g_pd3dDevice->SetStreamSource(0, g_pVB, 0,sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
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
		if (SUCCEEDED(InitGeometry()))
		{
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			MSG msg;
			ZeroMemory(&msg, sizeof(msg));
			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
					Render();

			}
		}
	}

	UnregisterClass(L"TwiLight", wc.hInstance);
	return 0;
}






