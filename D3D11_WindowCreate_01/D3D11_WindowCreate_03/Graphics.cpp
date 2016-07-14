#include "Graphics.h"

Graphics::Graphics()
{
	m_pD3D = nullptr;
}

Graphics::Graphics(const Graphics&)
{

}

Graphics::~Graphics()
{

}

bool Graphics::Initilize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	//Create D3D Object;
	m_pD3D = new D3DClass;
	if (!m_pD3D)
		return false;
	//Callback  D3D Init Function;
	result = m_pD3D->Initialize(screenWidth, screenHeight, Vsync_Enabled, hwnd, Full_Screen, Screen_Depth, Screen_Near);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}
	return true;
}

void Graphics::ShutDown()
{
	if (m_pD3D)
	{
		m_pD3D->Shutdown();
		delete m_pD3D;
		m_pD3D = nullptr;
	}
}

bool Graphics::Frame()
{
	bool result;
	result = Render();
	if (!result)
		return false;
	return true;
}

bool Graphics::Render()
{
	m_pD3D->BeginScene(0.0f, 0.0f, 0.5f, 1.0f);
	m_pD3D->EndScene();
	return true;
}


