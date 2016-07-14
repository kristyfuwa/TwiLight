#include <D3D9.h>
#include <d3dx9math.h>
#include <mmsystem.h>
#pragma warning(disable:4996)
#include <strsafe.h>
#pragma warning(disable:4996)

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9d.lib")
#pragma comment(lib,"winmm.lib")

//-----------------------------------------------------------------------------
// Desc: ȫ�ֱ���
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D = NULL;    //Direct3D����
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;    //Direct3D�豸����
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;    //���㻺��������
LPDIRECT3DVERTEXBUFFER9 g_pVB2 = NULL;    //���㻺��������
LPDIRECT3DTEXTURE9      g_pTexture = NULL;    //�������
LPDIRECT3DTEXTURE9		g_pTexture2 = nullptr;

LPDIRECT3DTEXTURE9      g_pRender2Tex = NULL;
LPDIRECT3DSURFACE9      g_pRender2Sur = NULL;
LPDIRECT3DTEXTURE9      g_pRenderTex = NULL;
LPDIRECT3DSURFACE9      g_pRenderSur = NULL;
LPDIRECT3DSURFACE9      g_pBackupSur = NULL;
LPD3DXRENDERTOSURFACE	g_pRTTInterface = nullptr;

//-----------------------------------------------------------------------------
// Desc: ����ṹ
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
	FLOAT x, y, z;    //����λ��  
	FLOAT u, v;          //������������
};
#define D3DFVF_CUSTOMVERTEX   (D3DFVF_XYZ|D3DFVF_TEX1)


//-----------------------------------------------------------------------------
// Desc: ���ñ任����
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
	//�����������������
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixRotationY(&matWorld, timeGetTime() / 2000.0f);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	//���������ù۲����
	D3DXVECTOR3 vEyePt(0.0f, 0.0f, -20);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	//����������ͶӰ����
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 2, 1.0f, 0.0f, 100.0f);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}


//-----------------------------------------------------------------------------
// Desc: ��ʼ��Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D(HWND hWnd)
{
	//����Direct3D����, �ö������ڴ���Direct3D�豸����
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	//����D3DPRESENT_PARAMETERS�ṹ, ׼������Direct3D�豸����
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;

	//����Direct3D�豸����
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	//��������Ч��
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	HRESULT hr = g_pd3dDevice->CreateTexture(256, 256, 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pRenderTex, NULL);
	if (FAILED(hr))
	{
		return E_FAIL;
	}
	g_pRenderTex->GetSurfaceLevel(0, &g_pRenderSur);

	hr = g_pd3dDevice->CreateTexture(256, 256, 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pRender2Tex, NULL);
	if (FAILED(hr))
	{
		return E_FAIL;
	}
	g_pRender2Tex->GetSurfaceLevel(0, &g_pRender2Sur);

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: ��������ͼ��
//-----------------------------------------------------------------------------
HRESULT InitGriphics()
{
	//�����������
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, L"banana.bmp", &g_pTexture)))
	{
		MessageBox(NULL, L"��������ʧ��", L"Texture.exe", MB_OK);
		return E_FAIL;
	}

	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, L"texture.bmp", &g_pTexture2)))
	{
		MessageBox(NULL, L"��������ʧ��", L"Texture.exe", MB_OK);
		return E_FAIL;
	}

	//��������
	CUSTOMVERTEX g_Vertices[] =
	{
		{ -10,   -10,  0.0f,  0.0f, 1.0f },
		{ -10,    10,  0.0f,  0.0f, 0.0f },
		{ 10,   -10,  0.0f,  1.0f, 1.0f },
		{ 10,    10,  0.0f,  1.0f, 0.0f }
	};

	//�������㻺����
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_MANAGED, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	//��䶥�㻺����
	VOID* pVertices;
	if (FAILED(g_pVB->Lock(0, sizeof(g_Vertices), (void**)&pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, g_Vertices, sizeof(g_Vertices));
	g_pVB->Unlock();


	//��������
	CUSTOMVERTEX g_Vertices2[] =
	{
		{ 10,   -10,  0.0f,  0.0f, 1.0f },
		{ 10,    10,  0.0f,  0.0f, 0.0f },
		{ 30,   -10,  0.0f,  1.0f, 1.0f },
		{ 30,    10,  0.0f,  1.0f, 0.0f }

	};

	//�������㻺����
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_MANAGED, &g_pVB2, NULL)))
	{
		return E_FAIL;
	}

	//��䶥�㻺����
	VOID* pVertices2;
	if (FAILED(g_pVB2->Lock(0, sizeof(g_Vertices2), (void**)&pVertices2, 0)))
		return E_FAIL;
	memcpy(pVertices2, g_Vertices2, sizeof(g_Vertices2));
	g_pVB2->Unlock();

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: �ͷŴ����Ķ���
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	//�ͷ��������
	if (g_pTexture != NULL)
		g_pTexture->Release();

	//�ͷŶ��㻺��������
	if (g_pVB != NULL)
		g_pVB->Release();

	//�ͷ�Direct3D�豸����
	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	//�ͷ�Direct3D����
	if (g_pD3D != NULL)
		g_pD3D->Release();
}


//-----------------------------------------------------------------------------
// Desc: ��Ⱦͼ�� 
//-----------------------------------------------------------------------------
VOID Render()
{
	//��պ�̨������
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(45, 50, 170), 1.0f, 0);

	//��ʼ�ں�̨����������ͼ��
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		SetupMatrices();
		g_pd3dDevice->GetRenderTarget(0, &g_pBackupSur);
		g_pd3dDevice->SetRenderTarget(0, g_pRenderSur);
		/*RTT*/
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		g_pd3dDevice->SetTexture(0, g_pTexture); //��������
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		D3DXSaveTextureToFileA("tt.tga", D3DXIFF_TGA, g_pRenderTex, NULL);

		D3DXMATRIXA16 matWorld1;
		D3DXMatrixIdentity(&matWorld1);
		D3DXMatrixRotationY(&matWorld1, timeGetTime() / 5000.0f);
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld1);

		g_pd3dDevice->SetRenderTarget(0, g_pRender2Sur);
		/*RTT*/
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		g_pd3dDevice->SetTexture(0, g_pTexture2); //��������
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		D3DXSaveTextureToFileA("tt2.tga", D3DXIFF_TGA, g_pRender2Tex, NULL);

		D3DXMATRIXA16 matWorld;
		D3DXMatrixIdentity(&matWorld);
		//D3DXMatrixRotationY(&matWorld, timeGetTime() / 2000.0f);
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

		//���������ù۲����
		D3DXVECTOR3 vEyePt(10.0f, 0.0f, -25);
		D3DXVECTOR3 vLookatPt(10.0f, 0.0f, 0.0f);
		D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
		D3DXMATRIXA16 matView;
		D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
		g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
		/*���ǳ���*/
		g_pd3dDevice->SetRenderTarget(0, g_pBackupSur);
		g_pd3dDevice->SetTexture(0, g_pRenderTex);
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		g_pd3dDevice->SetTexture(0, g_pRender2Tex);
		g_pd3dDevice->SetStreamSource(0, g_pVB2, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


		//�����ں�̨����������ͼ��
		g_pd3dDevice->EndScene();
	}

	//���ں�̨���������Ƶ�ͼ���ύ��ǰ̨��������ʾ
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}


//-----------------------------------------------------------------------------
// Desc: ��Ϣ����
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}



//-----------------------------------------------------------------------------
// Desc: ��ں���
//-----------------------------------------------------------------------------
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{

	//ע�ᴰ����
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"ClassName", NULL };
	RegisterClassEx(&wc);

	//��������
	HWND hWnd = CreateWindow(L"ClassName", L"����Ӱ�����",
		WS_OVERLAPPEDWINDOW, 200, 100, 800, 600,
		GetDesktopWindow(), NULL, wc.hInstance, NULL);

	//��ʼ��Direct3D
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		//��������ͼ��
		if (SUCCEEDED(InitGriphics()))
		{
			//��ʾ����
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			//������Ϣѭ��
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
				{
					Render();  //��Ⱦͼ��
				}
			}
		}
	}

	UnregisterClass(L"ClassName", wc.hInstance);
	return 0;
}






