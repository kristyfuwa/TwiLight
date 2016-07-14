#include <D3D9.h>
#include <d3dx9math.h>
#include <mmsystem.h>
#pragma warning(disable:4996)
#include <strsafe.h>
#pragma warning(disable:4996)
#include <openvr.h>
#include <string>
using namespace std;

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9d.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"openvr_api.lib")



LPDIRECT3D9				g_pD3D = nullptr;
LPDIRECT3DDEVICE9		g_pd3dDevice = nullptr;
LPDIRECT3DVERTEXBUFFER9	g_pVB = nullptr;

LPDIRECT3DTEXTURE9      g_pLeftTexture = NULL;    //纹理对象
LPDIRECT3DTEXTURE9		g_pRightTexture = NULL;

LPDIRECT3DTEXTURE9      g_pRenderLeftEyeTex = NULL;
LPDIRECT3DSURFACE9      g_pRenderLeftEyeSur = NULL;
LPDIRECT3DTEXTURE9      g_pRenderRightEyeTex = NULL;
LPDIRECT3DSURFACE9      g_pRenderRightEyeSur = NULL;
LPDIRECT3DSURFACE9      g_pBackupSur = NULL;

D3DXMATRIXA16			g_mat4HMDPose;
D3DXMATRIXA16			g_mat4eyePosLeft;
D3DXMATRIXA16		    g_mat4eyePosRight;
D3DXMATRIXA16			g_mat4ProjectionLeft;
D3DXMATRIXA16			g_mat4ProjectionRight;

FLOAT					g_fNearClip = 0.0f;
FLOAT					g_fFarClip = 30.0f;

struct CUSTOMVERTEX
{
	FLOAT	x, y, z;
	FLOAT	u, v;
};

//RHW是D3D里面的一种顶点格式，是一种自带视图、投影、世界等变换和光线计算等处理的顶点格式，
//而且这些处理我们无法用SetTransform等进行的转换。

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

//FVF;flexible vertex format

vr::IVRSystem*				m_pHMD = nullptr;
vr::IVRRenderModels*		m_pRenderModels = nullptr;
std::string					m_strDriver;
std::string					m_strDisplay;
UINT32						m_nRenderWidth;
UINT32						m_nRenderHeight;
HWND						m_hWnd;
vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];


HRESULT InitVR()
{
	vr::EVRInitError eError = vr::VRInitError_None;
	m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);
	if (eError != vr::VRInitError_None)
	{
		m_pHMD = nullptr;
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		return false;
	}
}

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


	int adapterIndex = m_pHMD->GetD3D9AdapterIndex();
	if (adapterIndex == -1)
	{
		return E_FAIL;
	}
	if (FAILED(g_pD3D->CreateDevice(adapterIndex, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	m_pHMD->GetRecommendedRenderTargetSize(&m_nRenderWidth, &m_nRenderHeight);


	if (FAILED(g_pd3dDevice->CreateTexture(m_nRenderWidth, m_nRenderHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pRenderLeftEyeTex, NULL)))
	{
		return E_FAIL;
	}
	g_pRenderLeftEyeTex->GetSurfaceLevel(0, &g_pRenderLeftEyeSur);

	if (FAILED(g_pd3dDevice->CreateTexture(m_nRenderWidth, m_nRenderHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pRenderRightEyeTex, NULL)))
	{
		return E_FAIL;
	}
	g_pRenderRightEyeTex->GetSurfaceLevel(0, &g_pRenderRightEyeSur);

	return S_OK;
}

BOOL InitCompositor()
{
	vr::EVRInitError peError = vr::VRInitError_None;

	if (!vr::VRCompositor())
	{
		printf("Compositor initialization failed. See log file for details\n");
		return false;
	}

	return true;
}

HRESULT InitGeometry()
{
	//创建纹理对象
	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, L"banana.bmp", &g_pLeftTexture)))
	{
		MessageBox(NULL, L"创建纹理失败", L"Texture.exe", MB_OK);
		return E_FAIL;
	}

	if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, L"texture.bmp", &g_pRightTexture)))
	{
		MessageBox(NULL, L"创建纹理失败", L"Texture.exe", MB_OK);
		return E_FAIL;
	}

	//顶点数据
	CUSTOMVERTEX g_Vertices[] =
	{
		{ -10,   -10,  0.0f,  0.0f, 1.0f },
		{ -10,    10,  0.0f,  0.0f, 0.0f },
		{ 10,   -10,  0.0f,  1.0f, 1.0f },
		{ 10,    10,  0.0f,  1.0f, 0.0f }
	};

	//创建顶点缓冲区
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_MANAGED, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	//填充顶点缓冲区
	VOID* pVertices;
	if (FAILED(g_pVB->Lock(0, sizeof(g_Vertices), (void**)&pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, g_Vertices, sizeof(g_Vertices));
	g_pVB->Unlock();


	return S_OK;
}

VOID Cleanup()
{
	if (m_pHMD != nullptr)
	{
		vr::VR_Shutdown();
		m_pHMD = nullptr;
	}

	if (g_pVB != nullptr)
		g_pVB->Release();

	if (g_pd3dDevice != nullptr)
		g_pd3dDevice->Release();

	if (g_pD3D != nullptr)
		g_pD3D->Release();
}

D3DXMATRIXA16 GetHMDMatrixProjectionEye(vr::Hmd_Eye nEye)
{
	if (!m_pHMD)
		return D3DXMATRIXA16();

	vr::HmdMatrix44_t mat = m_pHMD->GetProjectionMatrix(nEye, g_fNearClip, g_fFarClip, vr::API_OpenGL);

	return D3DXMATRIXA16(
		mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
		mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
		mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
		mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
		);
}

D3DXMATRIXA16 GetHMDMatrixPoseEye(vr::Hmd_Eye nEye)
{
	if (!m_pHMD)
		return D3DXMATRIXA16();

	vr::HmdMatrix34_t matEye = m_pHMD->GetEyeToHeadTransform(nEye);
	D3DXMATRIXA16 matrixObj(
		matEye.m[0][0], matEye.m[1][0], matEye.m[2][0], 0.0,
		matEye.m[0][1], matEye.m[1][1], matEye.m[2][1], 0.0,
		matEye.m[0][2], matEye.m[1][2], matEye.m[2][2], 0.0,
		matEye.m[0][3], matEye.m[1][3], matEye.m[2][3], 1.0f
		);
	D3DXMATRIXA16	matrixObjInverse;
	D3DXMatrixIdentity(&matrixObjInverse);
	D3DXMatrixInverse(&matrixObjInverse, NULL, &matrixObj);

	return matrixObjInverse;
}

VOID SetupCamera()
{
	g_mat4ProjectionLeft = GetHMDMatrixProjectionEye(vr::Eye_Left);
	g_mat4ProjectionRight = GetHMDMatrixProjectionEye(vr::Eye_Right);
	g_mat4eyePosLeft = GetHMDMatrixPoseEye(vr::Eye_Left);
	g_mat4eyePosRight = GetHMDMatrixPoseEye(vr::Eye_Right);
}

VOID SetupMatrices(vr::Hmd_Eye nEye)
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
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		g_pd3dDevice->GetRenderTarget(0, &g_pBackupSur);
		SetupMatrices(vr::Eye_Left);
		g_pd3dDevice->SetRenderTarget(0, g_pRenderLeftEyeSur);
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
		g_pd3dDevice->SetTexture(0, g_pLeftTexture); //设置纹理
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 1);
		D3DXSaveTextureToFileA("tt.tga", D3DXIFF_TGA, g_pRenderLeftEyeTex, NULL);

		SetupMatrices(vr::Eye_Right);
		g_pd3dDevice->SetRenderTarget(0, g_pRenderRightEyeSur);
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
		g_pd3dDevice->SetTexture(0, g_pRightTexture); //设置纹理
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 1);
		D3DXSaveTextureToFileA("tt2.tga", D3DXIFF_TGA, g_pRenderRightEyeTex, NULL);
	}

	g_pd3dDevice->Present(NULL, NULL, m_hWnd, NULL);
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

	//m_pHMD->GetRecommendedRenderTargetSize(&m_nRenderWidth, &m_nRenderHeight);
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, L"TwiLight", L"TwiLight", WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, GetDesktopWindow(), NULL, wc.hInstance, NULL);


	if (SUCCEEDED(InitD3D(m_hWnd) && InitVR()))
	{
		if (SUCCEEDED(InitCompositor()))
		{
			if (SUCCEEDED(InitGeometry()))
			{
				ShowWindow(m_hWnd, SW_SHOWDEFAULT);
				UpdateWindow(m_hWnd);

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
	}

	UnregisterClass(L"TwiLight", wc.hInstance);
	return 0;
}