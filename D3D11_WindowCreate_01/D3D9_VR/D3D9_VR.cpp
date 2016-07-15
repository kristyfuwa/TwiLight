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
LPDIRECT3DVERTEXBUFFER9 g_pVB2 = NULL;

LPDIRECT3DTEXTURE9      g_pLeftTexture = NULL;    //纹理对象
//LPDIRECT3DTEXTURE9		g_pRightTexture = NULL;

LPDIRECT3DTEXTURE9      g_pRenderLeftEyeTex = NULL;
LPDIRECT3DSURFACE9      g_pRenderLeftEyeSur = NULL;
//LPDIRECT3DTEXTURE9      g_pRenderRightEyeTex = NULL;
//LPDIRECT3DSURFACE9      g_pRenderRightEyeSur = NULL;
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
int							m_iValidPoseCount;
std::string					m_strPoseClasses;                            // what classes we saw poses for this frame
char						m_rDevClassChar[vr::k_unMaxTrackedDeviceCount];
vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
D3DXMATRIXA16 m_rmat4DevicePose[vr::k_unMaxTrackedDeviceCount];

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
	memset(m_rDevClassChar, 0, sizeof(m_rDevClassChar));
	return S_OK;
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

	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pd3dDevice)))
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

	//if (FAILED(g_pd3dDevice->CreateTexture(m_nRenderWidth, m_nRenderHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pRenderRightEyeTex, NULL)))
	//{
	//	return E_FAIL;
	//}
	//g_pRenderRightEyeTex->GetSurfaceLevel(0, &g_pRenderRightEyeSur);

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


	//if (FAILED(D3DXCreateTextureFromFile(g_pd3dDevice, L"texture.bmp", &g_pRightTexture)))
	//{
	//	MessageBox(NULL, L"创建纹理失败", L"Texture.exe", MB_OK);
	//	return E_FAIL;
	//}


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

	//顶点数据
	CUSTOMVERTEX g_Vertices2[] =
	{
		{ 10,   -10,  0.0f,  0.0f, 1.0f },
		{ 10,    10,  0.0f,  0.0f, 0.0f },
		{ 30,   -10,  0.0f,  1.0f, 1.0f },
		{ 30,    10,  0.0f,  1.0f, 0.0f }

	};

	//创建顶点缓冲区
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_MANAGED, &g_pVB2, NULL)))
	{
		return E_FAIL;
	}

	//填充顶点缓冲区
	VOID* pVertices2;
	if (FAILED(g_pVB2->Lock(0, sizeof(g_Vertices2), (void**)&pVertices2, 0)))
		return E_FAIL;
	memcpy(pVertices2, g_Vertices2, sizeof(g_Vertices2));
	g_pVB2->Unlock();
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

D3DXMATRIXA16 GetCurrentViewProjectionMatrix(vr::Hmd_Eye nEye)
{
	D3DXMATRIXA16 matMVP;
	if (nEye == vr::Eye_Left)
	{
		matMVP = g_mat4ProjectionLeft * g_mat4eyePosLeft * g_mat4HMDPose;
	}
	else if (nEye == vr::Eye_Right)
	{
		matMVP = g_mat4ProjectionRight * g_mat4eyePosRight *  g_mat4HMDPose;
	}

	return matMVP;
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

VOID SetupMatrices()
{
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity(&matWorld);
	D3DXMatrixRotationY(&matWorld, timeGetTime() / 2000.0f);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	//创建并设置观察矩阵
	D3DXVECTOR3 vEyePt(0.0f, 0.0f, -20);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&matView, &vEyePt, &vLookatPt, &vUpVec);
	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);

	//创建并设置投影矩阵
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 2, 1.0f, 0.0f, 100.0f);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

D3DXMATRIXA16 ConvertSteamVRMatrixToMatrix4(const vr::HmdMatrix34_t &matPose)
{
	D3DXMATRIXA16 matrixObj(
		matPose.m[0][0], matPose.m[1][0], matPose.m[2][0], 0.0,
		matPose.m[0][1], matPose.m[1][1], matPose.m[2][1], 0.0,
		matPose.m[0][2], matPose.m[1][2], matPose.m[2][2], 0.0,
		matPose.m[0][3], matPose.m[1][3], matPose.m[2][3], 1.0f
	);
	return matrixObj;
}

void UpdateHMDMatrixPose()
{
	if (!m_pHMD)
	{
		return;
	}
	vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
	m_iValidPoseCount = 0;
	m_strPoseClasses = "";

	for (int nDevice = 0;nDevice < vr::k_unMaxTrackedDeviceCount;++nDevice)
	{
		if (m_rTrackedDevicePose[nDevice].bPoseIsValid)
		{
			m_iValidPoseCount++;
			m_rmat4DevicePose[nDevice] = ConvertSteamVRMatrixToMatrix4(m_rTrackedDevicePose[nDevice].mDeviceToAbsoluteTracking);
			if (m_rDevClassChar[nDevice] == 0)
			{
				switch (m_pHMD->GetTrackedDeviceClass(nDevice))
				{
				case vr::TrackedDeviceClass_Controller:        m_rDevClassChar[nDevice] = 'C'; break;
				case vr::TrackedDeviceClass_HMD:               m_rDevClassChar[nDevice] = 'H'; break;
				case vr::TrackedDeviceClass_Invalid:           m_rDevClassChar[nDevice] = 'I'; break;
				case vr::TrackedDeviceClass_Other:             m_rDevClassChar[nDevice] = 'O'; break;
				case vr::TrackedDeviceClass_TrackingReference: m_rDevClassChar[nDevice] = 'T'; break;
				default:                                       m_rDevClassChar[nDevice] = '?'; break;
				}
			}
			m_strPoseClasses += m_rDevClassChar[nDevice];
		}
	}
	if (m_rTrackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid)
	{
		D3DXMATRIXA16	matrixInverse;
		D3DXMatrixIdentity(&matrixInverse);
		D3DXMatrixInverse(&matrixInverse, NULL, &m_rmat4DevicePose[vr::k_unTrackedDeviceIndex_Hmd]);
		g_mat4HMDPose = matrixInverse;
	}
}

VOID Render()
{
	if (NULL == g_pd3dDevice)
		return;
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		//SetupMatrices();
		//g_pd3dDevice->GetRenderTarget(0, &g_pBackupSur);

		//g_pd3dDevice->SetRenderTarget(0, g_pRenderLeftEyeSur);
		//g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
		//g_pd3dDevice->SetTexture(0, g_pLeftTexture); //设置纹理
		//g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		//g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		//D3DXSaveTextureToFileA("tt.tga", D3DXIFF_TGA, g_pRenderLeftEyeTex, NULL);

		//g_pd3dDevice->SetRenderTarget(0, g_pRenderRightEyeSur);
		//g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
		//g_pd3dDevice->SetTexture(0, g_pRightTexture); //设置纹理
		//g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		//g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		//D3DXSaveTextureToFileA("tt2.tga", D3DXIFF_TGA, g_pRenderRightEyeTex, NULL);

		//D3DXMATRIXA16 matWorld1;
		//D3DXMatrixIdentity(&matWorld1);
		//D3DXMatrixRotationY(&matWorld1, timeGetTime() / 500.0f);
		//g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld1);
		//g_pd3dDevice->SetRenderTarget(0, g_pRenderRightEyeSur);
		//g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
		//g_pd3dDevice->SetTexture(0, g_pRightTexture); //设置纹理
		//g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		//g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		//D3DXSaveTextureToFileA("tt2.tga", D3DXIFF_TGA, g_pRenderRightEyeTex, NULL);

		vr::Texture_t leftEyeTexture = { (void*)g_pLeftTexture,vr::API_DirectX,vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
		vr::Texture_t rightEyeTexture = { (void*)g_pLeftTexture,vr::API_DirectX,vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

		D3DXMATRIXA16 matViewLeft = g_mat4eyePosLeft*g_mat4HMDPose;
		g_pd3dDevice->SetTransform(D3DTS_VIEW, &matViewLeft);
		D3DXMATRIXA16 matProjLeft = g_mat4ProjectionLeft; 
		g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProjLeft);
		g_pd3dDevice->SetTexture(0, g_pLeftTexture);
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		D3DXMATRIXA16 matViewRight = g_mat4eyePosRight*g_mat4HMDPose;
		g_pd3dDevice->SetTransform(D3DTS_VIEW, &matViewRight);
		D3DXMATRIXA16 matProjRight = g_mat4ProjectionRight;
		g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProjRight);
		g_pd3dDevice->SetTexture(0, g_pLeftTexture);
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		//D3DXMATRIXA16 matProjLeft = GetCurrentViewProjectionMatrix(vr::Eye_Left);
		//g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProjLeft);
		//g_pd3dDevice->SetTexture(0, g_pLeftTexture);
		//g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		//g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		//D3DXMATRIXA16 matProjRight = GetCurrentViewProjectionMatrix(vr::Eye_Right);
		//g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProjRight);
		//g_pd3dDevice->SetTexture(0, g_pLeftTexture);
		//g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		//g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		//g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		g_pd3dDevice->EndScene();
	}
	g_pd3dDevice->Present(NULL, NULL, m_hWnd, NULL);

	UpdateHMDMatrixPose();
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


	if (SUCCEEDED(InitVR()))
	{
		if (SUCCEEDED(InitD3D(m_hWnd)))
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
	}

	UnregisterClass(L"TwiLight", wc.hInstance);
	return 0;
}