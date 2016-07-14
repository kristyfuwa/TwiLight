#pragma once

#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dx11.lib")
#pragma comment(lib,"d3dx10.lib")

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>

class D3DClass
{
public:
	D3DClass();
	~D3DClass();
	bool Initialize(int screenWidth, int screenHeight, bool vsync_enable, HWND hwnd, bool fullscreen, float screendepth, float screennear);
	void Shutdown();
	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(D3DXMATRIX&);
	void GetWorldMatrix(D3DXMATRIX&);
	void GetOrthoMatrix(D3DXMATRIX&);

	void GetVideoCardInfo(char*, int&);
private:
	bool	m_vsync_enabled;
	int	m_videoCardMemory;
	char	m_videoCardDescroption[128];
	IDXGISwapChain*	m_pSwapChain;//交换链对象
	ID3D11Device	*m_pDevice;//设备对象
	ID3D11DeviceContext*	m_pDeviceContext;//设备上下文对象

	ID3D11RenderTargetView* m_renderTargetView; //渲染目标视图
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView; //深度目标视图
	ID3D11RasterizerState* m_rasterState; //渲染状态
	D3DXMATRIX m_projectionMatrix; //投影矩阵
	D3DXMATRIX m_worldMatrix;//世界坐标系矩阵
	D3DXMATRIX m_orthoMatrix;//正交投影矩阵

};

