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
struct CUSTOMVERTEX
{
	FLOAT	x, y, z;
	DWORD	color;
};

//RHW是D3D里面的一种顶点格式，是一种自带视图、投影、世界等变换和光线计算等处理的顶点格式，
//而且这些处理我们无法用SetTransform等进行的转换。

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

//FVF;flexible vertex format

vr::IVRSystem*				m_pHMD = nullptr;
vr::IVRRenderModels*		m_pRenderModels = nullptr;
std::string					m_strDriver;
std::string					m_strDisplay;
UINT32						m_nRenderWidth;
UINT32						m_nRenderHeight;
D3DXMATRIXA16				matrixObjLeftInverse;
D3DXMATRIXA16				matrixObjRightInverse;
HWND						m_hWnd;
vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];

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

VOID SetupMatrices(vr::Hmd_Eye nEye)
{

	vr::HmdMatrix44_t	mat = m_pHMD->GetProjectionMatrix(vr::Eye_Left, 0.0f, 100.0f, vr::API_DirectX);
	D3DXMATRIXA16 proMatrixLeft(mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
		mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
		mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
		mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]);
	vr::HmdMatrix44_t	mat2 = m_pHMD->GetProjectionMatrix(vr::Eye_Right, 0.0f, 100.0f, vr::API_DirectX);
	D3DXMATRIXA16 proMatrixRight(mat2.m[0][0], mat2.m[1][0], mat2.m[2][0], mat2.m[3][0],
		mat2.m[0][1], mat2.m[1][1], mat2.m[2][1], mat2.m[3][1],
		mat2.m[0][2], mat2.m[1][2], mat2.m[2][2], mat2.m[3][2],
		mat2.m[0][3], mat2.m[1][3], mat2.m[2][3], mat2.m[3][3]);

	vr::HmdMatrix34_t matEyeLeft = m_pHMD->GetEyeToHeadTransform(vr::Eye_Left);
	D3DXMATRIXA16 matrixObjLeft(
		matEyeLeft.m[0][0], matEyeLeft.m[1][0], matEyeLeft.m[2][0], 0.0,
		matEyeLeft.m[0][1], matEyeLeft.m[1][1], matEyeLeft.m[2][1], 0.0,
		matEyeLeft.m[0][2], matEyeLeft.m[1][2], matEyeLeft.m[2][2], 0.0,
		matEyeLeft.m[0][3], matEyeLeft.m[1][3], matEyeLeft.m[2][3], 1.0f);
	D3DXMatrixInverse(&matrixObjLeftInverse, NULL, &matrixObjLeft);

	vr::HmdMatrix34_t matEyeRight = m_pHMD->GetEyeToHeadTransform(vr::Eye_Left);
	D3DXMATRIXA16 matrixObjRight(
		matEyeRight.m[0][0], matEyeRight.m[1][0], matEyeRight.m[2][0], 0.0,
		matEyeRight.m[0][1], matEyeRight.m[1][1], matEyeRight.m[2][1], 0.0,
		matEyeRight.m[0][2], matEyeRight.m[1][2], matEyeRight.m[2][2], 0.0,
		matEyeRight.m[0][3], matEyeRight.m[1][3], matEyeRight.m[2][3], 1.0f);
	D3DXMatrixInverse(&matrixObjRightInverse, NULL, &matrixObjRight);



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
	if (nEye == vr::Eye_Left)
	{
		matProj = proMatrixLeft*matrixObjLeftInverse;
	}
	else if (nEye == vr::Eye_Right)
	{
		matProj = proMatrixRight*matrixObjRightInverse;
	}

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
		SetupMatrices(vr::Eye_Left);

		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
		g_pd3dDevice->EndScene();
	}

	g_pd3dDevice->Present(NULL, NULL, m_hWnd, NULL);

	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		SetupMatrices(vr::Eye_Right);
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

		g_pd3dDevice->EndScene();
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

std::string GetTrackedDeviceString(vr::IVRSystem *pHmd, vr::TrackedDeviceIndex_t unDevice, vr::TrackedDeviceProperty prop, vr::TrackedPropertyError *peError = NULL)
{
	uint32_t unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, NULL, 0, peError);
	if (unRequiredBufferLen == 0)
		return "";

	char *pchBuffer = new char[unRequiredBufferLen];
	unRequiredBufferLen = pHmd->GetStringTrackedDeviceProperty(unDevice, prop, pchBuffer, unRequiredBufferLen, peError);
	std::string sResult = pchBuffer;
	delete[] pchBuffer;
	return sResult;
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, INT)
{
	UNREFERENCED_PARAMETER(hInst);
	//告诉编译器，已经使用了该变量，不必检测警告！

	WNDCLASSEX wc = { sizeof(WNDCLASSEX),CS_CLASSDC,MsgPro,0L,0L,GetModuleHandle(NULL),NULL,NULL,NULL,NULL,L"TwiLight",NULL };

	RegisterClassEx(&wc);

	vr::EVRInitError eError = vr::VRInitError_None;
	m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);
	if (eError != vr::VRInitError_None)
	{
		m_pHMD = nullptr;
		char buf[1024];
		sprintf_s(buf, sizeof(buf), "Unable to init VR runtime: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		return false;
	}

	m_pRenderModels = (vr::IVRRenderModels*)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError);
	if (!m_pRenderModels)
	{
		m_pHMD = NULL;
		vr::VR_Shutdown();

		char buf[1024];
		sprintf_s(buf, sizeof(buf), "Unable to get render model interface: %s", vr::VR_GetVRInitErrorAsEnglishDescription(eError));
		return false;
	}

	m_strDriver = GetTrackedDeviceString(m_pHMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_TrackingSystemName_String);
	m_strDisplay = GetTrackedDeviceString(m_pHMD, vr::k_unTrackedDeviceIndex_Hmd, vr::Prop_SerialNumber_String);

	m_pHMD->GetRecommendedRenderTargetSize(&m_nRenderWidth, &m_nRenderHeight);
	m_hWnd = CreateWindowEx(WS_EX_APPWINDOW, L"TwiLight", L"TwiLight", WS_OVERLAPPEDWINDOW, 0, 0, m_nRenderWidth, m_nRenderHeight, GetDesktopWindow(), NULL, wc.hInstance, NULL);

	if (!vr::VRCompositor())
	{
		printf("Compositor initialization failed. See log file for details\n");
		return false;
	}


	if (SUCCEEDED(InitD3D(m_hWnd)))
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

	UnregisterClass(L"TwiLight", wc.hInstance);
	return 0;
}