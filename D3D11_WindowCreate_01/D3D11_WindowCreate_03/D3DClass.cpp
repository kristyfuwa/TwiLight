#include "D3DClass.h"



D3DClass::D3DClass()
{
	m_pSwapChain = 0;
	m_pDevice = 0;
	m_pDeviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}


D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync_enable, HWND hwnd, bool fullscreen, float screendepth, float screennear)
{
	HRESULT		result;
	IDXGIFactory*	factory;
	IDXGIAdapter*	adpter;
	IDXGIOutput*	adapterOutput;
	unsigned int numModes, i, numerator/*����*/, denominator/*��ĸ*/, stringLength;
	DXGI_MODE_DESC*	displayModeList;
	DXGI_ADAPTER_DESC	adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC	swapChainDesc;
	D3D_FEATURE_LEVEL		featureLevel;
	ID3D11Texture2D*		backBufferPtr;
	D3D11_TEXTURE2D_DESC	depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC	depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC		rasterDesc;//��դ
	D3D11_VIEWPORT			viewport;
	float fieldOfView, screenAspect;

	m_vsync_enabled = vsync_enable;
	//Create a DirectX graphics interface factor
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
		return false;

	// �ýӿڹ�������һ�����Կ�������
	result = factory->EnumAdapters(0, &adpter);
	if (FAILED(result))
		return false;

	// �õ��������������.
	result = adpter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
		return false;

	//�õ��ʺ� DXGI_FORMAT_R8G8B8A8_UNORM ����ʾģʽ. 
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
		return false;
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
		return false;
	// ������ʾģʽ��displayModeList�� 
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
		return false;
	//����������ʾģʽ���õ�ˢ������������ֵ numerator �� denominator 
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//�õ��Կ�����
	result = adpter->GetDesc(&adapterDesc);
	if (FAILED(result))
		return false;
	//�����Դ��С
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//�����Կ�������
	//wcstombs_s wide char ת��Ϊchar
	error = wcstombs_s(&stringLength, m_videoCardDescroption, adapterDesc.Description, 128);
	if (error != 0)
		return false;
	//�ͷ���ʾģʽ�б�
	delete[]displayModeList;
	displayModeList = nullptr;

	//�ͷ����������
	adpter->Release();
	adpter = 0;

	//�ͷŽӿڹ���
	factory->Release();
	factory = nullptr;

	//��ʼ������������;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//ʹ��һ���󻺳�
	swapChainDesc.BufferCount = 1;

	//֡����Ĵ�С��Ӧ�ó��򴰿ڴ�С���  
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	// �󻺳��surface�ĸ�ʽΪDXGI_FORMAT_R8G8B8A8_UNORM   
	// surface��ÿ��������4���޷��ŵ�8bit[ӳ�䵽0-1]����ʾ��NORM��ʾ��һ��  
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ���ʹ�ô�ֱͬ�������ú󻺳��ˢ����

	//ˢ���ʾ���һ���ӰѺ󻺳���������Ļ�ϻ����Ĵ���  
	//���������ֱͬ����������ˢ���ʣ���fps�ǹ̶���

	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// ���ú󻺳����; 
	// ���ǵ���ȾĿ�껺��Ϊ�󻺳�  
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;

	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}
	// �趨ɨ����ordering�Լ�����Ϊunspecified  

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// �󻺳����ݳ��ֵ���Ļ�󣬷��������� 

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//�����־
	swapChainDesc.Flags = 0;
	// ����feature levelΪD3D11 
	// ����Կ���֧��D3D11,�����ܹ�ͨ���������������ʹ��D3D10,����9  
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// �������������豸�Լ��豸������  
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_pSwapChain, &m_pDevice, NULL,&m_pDeviceContext);
	if (FAILED(result))
		return false;
	//// �õ��������еĺ󻺳�ָ��.
	result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}


	result = m_pDevice->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
		return false;
	backBufferPtr->Release();
	backBufferPtr = nullptr;

	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	//������Ȼ������� 
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1; //������Ȼ���Ϊ1
	depthBufferDesc.ArraySize = 1;//������Ȼ���Ϊ1��������������2�������и�����; 
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//Create Depth Buffer;
	result = m_pDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
		return false;

	// ��ʼ�����ģ��״̬���� 
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask =  D3D11_DEPTH_WRITE_MASK_ALL; //D3D11_DEPTH_WRITE_MASK_ZERO��ֹд��Ȼ���
	
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.StencilReadMask = 0xFF;

	// ����front face ����ʹ�õ�ģ���������.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// ����back face����ʹ�õ�ģ�����ģʽ.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;

	}

	// �������ģ��״̬.
	m_pDeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// ��ʼ�����ģ����ͼ.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// �������ģ����ͼ����.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// �������ģ����ͼ.
	result = m_pDevice->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}
	m_pDeviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);


	// ���ù�դ��������ָ���������α���Ⱦ.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// ������դ��״̬
	result = m_pDevice->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
		return false;

	//���ù�դ��״̬��ʹ����Ч
	m_pDeviceContext->RSSetState(m_rasterState);

	// �����ӿڣ���ʾȫ���󻺳�����
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//Create View
	m_pDeviceContext->RSSetViewports(1, &viewport);

	//����͸��ͶӰ����
	fieldOfView = (float)D3DX_PI/4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// ����͸��ͶӰ����.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screennear, screendepth);
	//��ʼ��world����Ϊ��λ����.
	//�þ���ʵ�־ֲ����굽���������ת��
	D3DXMatrixIdentity(&m_worldMatrix);

	//// ��������ͶӰ������Ҫ����ʵʩ2D��Ⱦ.
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screennear, screendepth);

	return true;
}

void D3DClass::Shutdown()
{
	// �ͷŽ�������Դǰ��������Ϊ����ģʽ��������ܻ�����쳣.

	if (m_pSwapChain)
	{
		m_pSwapChain->SetFullscreenState(false, NULL);
	}
	if (m_rasterState)
	{
		m_rasterState->Release();
		m_rasterState = nullptr;
	}
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = nullptr;
	}
	if (m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = nullptr;
	}
	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_pDeviceContext)
	{
		m_pDeviceContext->Release();
		m_pDeviceContext = 0;
	}

	if (m_pDevice)
	{
		m_pDevice->Release();
		m_pDevice = 0;
	}

	if (m_pSwapChain)
	{
		m_pSwapChain->Release();
		m_pSwapChain = 0;
	}

	return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];
	// ��������󻺳���ɫ.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//����󻺳�.
	m_pDeviceContext->ClearRenderTargetView(m_renderTargetView, color);
	//�����Ȼ���.
	m_pDeviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
{
	//��Ⱦ��ɺ󣬰Ѻ󻺳�����present��ǰ����
	if (m_vsync_enabled)
	{
		// ������Ļˢ����.
		m_pSwapChain->Present(1, 0);
	}
	else
	{
		m_pSwapChain->Present(0, 0);
	}
	return;
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_pDevice;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_pDeviceContext;
}

void D3DClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
}

void D3DClass::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_worldMatrix;
}

void D3DClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_orthoMatrix;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescroption);
	memory = m_videoCardMemory;
}