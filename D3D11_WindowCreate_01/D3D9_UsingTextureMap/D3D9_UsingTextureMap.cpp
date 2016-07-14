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
LPDIRECT3DTEXTURE9		g_pTexture = nullptr;
HWND					g_Hwnd = nullptr;

struct CUSTOMVERTEX
{
	D3DXVECTOR3	position;
	D3DCOLOR	color;
#ifndef SHOW_HOW_TO_USE_TCI
	FLOAT	tu, tv;
#endif
};

#ifdef SHOW_HOW_TO_USE_TCI
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
#else
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#endif


//RHW是D3D里面的一种顶点格式，是一种自带视图、投影、世界等变换和光线计算等处理的顶点格式，
//而且这些处理我们无法用SetTransform等进行的转换。


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
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	return S_OK;
}

HRESULT InitGeometry()
{
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, L"banana.bmp", &g_pTexture)))
	{
		// If texture is not in current folder, try parent folder
		if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, L"..\\banana.bmp", &g_pTexture)))
		{
			MessageBox(NULL, L"Could not find banana.bmp", L"Textures.exe", MB_OK);
			return E_FAIL;
		}
	}

	if (FAILED(g_pd3dDevice->CreateVertexBuffer(50 * 2 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	CUSTOMVERTEX* pVertices;
	if (FAILED(g_pVB->Lock(0,0, (void**)&pVertices, 0)))
	{
		return E_FAIL;
	}

	for (DWORD i = 0; i < 50; i++)
	{
		//glVertex2f(R*cos(2 * Pi / n*i), R*sin(2 * Pi / n*i));

		FLOAT theta = (2 * D3DX_PI*i) / (50 - 1);
		pVertices[2 * i + 0].position = D3DXVECTOR3(sinf(2 * D3DX_PI*i/49), -1.0f, cosf(2 * D3DX_PI*i/49));
		pVertices[2 * i + 0].color = 0xffffffff;
#ifndef SHOW_HOW TO_USE_TCI
		pVertices[2*i+0].tu = ((FLOAT)i) / (50 - 1);
		pVertices[2 * i + 0].tv = 1.0f;
#endif

		pVertices[2 * i + 1].position = D3DXVECTOR3(sinf(2 * D3DX_PI*i/49), 1.0f, cosf(2 * D3DX_PI*i/49));
		pVertices[2 * i + 1].color = 0xff808080;//128，128，128
#ifndef SHOW_HOW_TO_USE_TCI
		pVertices[2 * i + 1].tu = ((FLOAT)i) / (50 - 1);
		pVertices[2 * i + 1].tv = 0.0f;
#endif
	}

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
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixRotationX(&matWorld, timeGetTime() / 1000.0f);
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


VOID SetLights()
{
	D3DMATERIAL9	mtrl;
	ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 0.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	//环境光颜色(Ambient)、漫反射颜色(Diffuse)、高光颜色(specular)
	g_pd3dDevice->SetMaterial(&mtrl);

	D3DXVECTOR3	vecDir;
	D3DLIGHT9	light;
	ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r = 1.0f;
	light.Diffuse.g = 1.0f;
	light.Diffuse.b = 1.0f;

	vecDir = D3DXVECTOR3(cosf(timeGetTime() / 350.0f),
		1.0f, sinf(timeGetTime() / 350.0f));
	D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecDir);

	light.Range = 1000.0f;
	g_pd3dDevice->SetLight(0, &light);
	g_pd3dDevice->LightEnable(0, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0x00202020);
}
VOID Render()
{
	if (NULL == g_pd3dDevice)
		return;
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		//SetLights();
		SetupMatrices();

		g_pd3dDevice->SetTexture(0, g_pTexture);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

#ifdef SHOW_HOW_TO_USE_TCI

		D3DXMATRIXA16 mTextureTransform;
		D3DXMATRIXA16 mProj;
		D3DXMATRIXA16 mTrans;
		D3DXMATRIXA16 mScale;

		g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &mProj);
		D3DXMatrixTranslation(&mTrans, 0.5f, 0.5f, 0.0f);
		D3DXMatrixScaling(&mScale, 0.5f, -0.5f, 1.0f);
		mTextureTransform = mProj * mScale * mTrans;

		g_pd3dDevice->SetTransform(D3DTS_TEXTURE0, &mTextureTransform);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT4 | D3DTTFF_PROJECTED);
		g_pd3dDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
#endif
		

		g_pd3dDevice->SetStreamSource(0, g_pVB, 0,sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 * 50 - 2);
		g_pd3dDevice->EndScene();
	}

	g_pd3dDevice->Present(NULL, NULL, g_Hwnd, NULL);
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

	g_Hwnd = CreateWindowEx(WS_EX_APPWINDOW, L"TwiLight", L"TwiLight", WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, GetDesktopWindow(), NULL, wc.hInstance, NULL);

	if (SUCCEEDED(InitD3D(g_Hwnd)))
	{
		if (SUCCEEDED(InitGeometry()))
		{
			ShowWindow(g_Hwnd, SW_SHOWDEFAULT);
			UpdateWindow(g_Hwnd);

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






