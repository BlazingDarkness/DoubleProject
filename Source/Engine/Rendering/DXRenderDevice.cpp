#include "Rendering\DXRenderDevice.h"

namespace Render
{
	///////////////////////////
	// Construct / destruction

	//Creates a device object but does not initialise
	DXRenderDevice::DXRenderDevice()
	{
		//nothing
	}

	//Releases all memory used
	DXRenderDevice::~DXRenderDevice()
	{
		if(m_pSceneManager != nullptr) delete m_pSceneManager;
		if (m_pModelShader != nullptr) delete m_pModelShader;
		if (m_pModelShader != nullptr) delete m_pDepthShader;

		// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
		if (m_pSwapChain)
		{
			m_pSwapChain->SetFullscreenState(false, NULL);
		}

		SAFE_RELEASE(m_pMatrixBuffer);
		SAFE_RELEASE(m_pRasterState);
		SAFE_RELEASE(m_pDepthStencilView);
		SAFE_RELEASE(m_pDepthStencilState);
		SAFE_RELEASE(m_pDepthStencilBuffer);
		SAFE_RELEASE(m_pRenderTargetView);
		SAFE_RELEASE(m_pDeviceContext);
		SAFE_RELEASE(m_pDevice);
		SAFE_RELEASE(m_pSwapChain);
	}

	//Initialises the device
	bool DXRenderDevice::Init(HWND hWnd)
	{
		/////////////////////////////////////////////////////////////////
		//D3D setup taken from an old DX10 projectr and updated to DX11//
		/////////////////////////////////////////////////////////////////


		// Many DirectX functions return a "HRESULT" variable to indicate success or failure. Microsoft code often uses
		// the FAILED macro to test this variable, you'll see it throughout the code - it's fairly self explanatory.
		HRESULT hr = S_OK;


		////////////////////////////////
		// Initialise Direct3D

		// Calculate the visible area the window we are using - the "client rectangle" refered to in the first function is the 
		// size of the interior of the window, i.e. excluding the frame and title
		RECT rc;
		GetClientRect(hWnd, &rc);
		m_ScreenWidth = rc.right - rc.left;
		m_ScreenHeight = rc.bottom - rc.top;

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;

		// Create a Direct3D device (i.e. initialise D3D), and create a swap-chain (create a back buffer to render to)
		DXGI_SWAP_CHAIN_DESC sd;         // Structure to contain all the information needed
		ZeroMemory(&sd, sizeof(sd)); // Clear the structure to 0 - common Microsoft practice, not really good style
		sd.BufferCount = 1;
		sd.BufferDesc.Width = m_ScreenWidth;             // Target window size
		sd.BufferDesc.Height = m_ScreenHeight;           // --"--
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Pixel format of target window
		sd.BufferDesc.RefreshRate.Numerator = 60;          // Refresh rate of monitor
		sd.BufferDesc.RefreshRate.Denominator = 1;         // --"--
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.OutputWindow = hWnd;                            // Target window
		sd.Windowed = TRUE;                                // Whether to render in a window (TRUE) or go fullscreen (FALSE)
		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
			D3D11_SDK_VERSION, &sd, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext);
		if (FAILED(hr)) return false;


		// Specify the render target as the back-buffer - this is an advanced topic. This code almost always occurs in the standard D3D setup
		ID3D11Texture2D* pBackBuffer;
		hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (FAILED(hr)) return false;
		hr = m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pRenderTargetView);
		pBackBuffer->Release();
		if (FAILED(hr)) return false;


		// Create a texture (bitmap) to use for a depth buffer
		D3D11_TEXTURE2D_DESC descDepth;
		ZeroMemory(&descDepth, sizeof(descDepth));
		descDepth.Width = m_ScreenWidth;
		descDepth.Height = m_ScreenHeight;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		hr = m_pDevice->CreateTexture2D(&descDepth, NULL, &m_pDepthStencilBuffer);
		if (FAILED(hr)) return false;

		// Create the depth stencil state
		D3D11_DEPTH_STENCIL_DESC descDSS;
		ZeroMemory(&descDSS, sizeof(descDSS));

		// Set up the description of the stencil state.
		descDSS.DepthEnable = true;
		descDSS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		descDSS.DepthFunc = D3D11_COMPARISON_LESS;

		descDSS.StencilEnable = true;
		descDSS.StencilReadMask = 0xFF;
		descDSS.StencilWriteMask = 0xFF;

		// Stencil operations if pixel is front-facing.
		descDSS.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDSS.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		descDSS.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDSS.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Stencil operations if pixel is back-facing.
		descDSS.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		descDSS.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		descDSS.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		descDSS.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		// Create the depth stencil state.
		hr = m_pDevice->CreateDepthStencilState(&descDSS, &m_pDepthStencilState);
		if (FAILED(hr))
		{
			return false;
		}

		// Set the depth stencil state.
		m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);


		// Create the depth stencil view, i.e. indicate that the texture just created is to be used as a depth buffer
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(descDSV));
		descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &descDSV, &m_pDepthStencilView);
		if (FAILED(hr)) return false;

		// Select the back buffer and depth buffer to use for rendering now
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		// Setup the raster description which will determine how and what polygons will be drawn.
		D3D11_RASTERIZER_DESC descRaster;
		ZeroMemory(&descRaster, sizeof(descRaster));
		descRaster.AntialiasedLineEnable = false;
		descRaster.CullMode = D3D11_CULL_BACK;
		descRaster.DepthBias = 0;
		descRaster.DepthBiasClamp = 0.0f;
		descRaster.DepthClipEnable = true;
		descRaster.FillMode = D3D11_FILL_SOLID;
		descRaster.FrontCounterClockwise = false;
		descRaster.MultisampleEnable = false;
		descRaster.ScissorEnable = false;
		descRaster.SlopeScaledDepthBias = 0.0f;

		hr = m_pDevice->CreateRasterizerState(&descRaster, &m_pRasterState);
		if (FAILED(hr))
		{
			return false;
		}


		// Setup the viewport - defines which part of the window we will render to, almost always the whole window
		D3D11_VIEWPORT vp;
		ZeroMemory(&vp, sizeof(vp));
		vp.Width = static_cast<float>(m_ScreenWidth);
		vp.Height = static_cast<float>(m_ScreenHeight);
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_pDeviceContext->RSSetViewports(1, &vp);

		m_pModelShader = new ModelShader;
		if (!m_pModelShader->Init(m_pDevice, ".\\ModelVS.cso", ".\\ModelPS.cso"))
		{
			return false;
		}

		m_pDepthShader = new DepthShader;
		if (!m_pDepthShader->Init(m_pDevice, ".\\DepthVS.cso", ".\\DepthPS.cso"))
		{
			return false;
		}


		D3D11_BUFFER_DESC cbDesc;
		cbDesc.ByteWidth = 192; // Constant buffer data is packed into float4 data - must round up to size of float4 (16)
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU is only going to write to the constants (not read them)
		cbDesc.MiscFlags = 0;
		if (FAILED(m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pMatrixBuffer)))
		{
			return false;
		}

		m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pMatrixBuffer);


		m_pMeshManager = new MeshManager(m_pDevice);
		m_pSceneManager = new Scene::Manager(m_pMeshManager);

		return true;
	}


	///////////////////////////
	// Rendering

	//Renders the scene
	void DXRenderDevice::RenderScene()
	{
		ClearScreen();

		///////////////////////////
		// Depth pre pass

		//m_Matrices.view = m_pSceneManager->GetActiveCamera()->GetViewMatrix();
		//m_Matrices.projection = 

		//D3D11_MAPPED_SUBRESOURCE s;
		//m_pDeviceContext->Map(m_pMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &s);

		m_pDepthShader->SetTechnique(m_pDeviceContext);
		SetConstantBuffer(m_pMatrixBuffer, 0, &m_Matrices, sizeof(MatrixBuffer), ShaderType::Vertex);


		///////////////////////////
		// Lighting pass

		m_pModelShader->SetShader(m_pDeviceContext);


		m_pSwapChain->Present(0, 0);
	}


	///////////////////////////
	// Render steps

	//Resets the back buffer and depth buffers
	void DXRenderDevice::ClearScreen()
	{
		float ClearColor[4] = { 0.2f, 0.2f, 0.3f, 1.0f };
		
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, ClearColor);

		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	}


	//Copys the data to a constant buffer
	void DXRenderDevice::SetConstantBuffer(ID3D11Buffer* buffer, unsigned int bufferIndex, void* data, unsigned int dataSize, ShaderType sType)
	{
		//This function's code was taken in part from Rastertek's tutorial dx11s2tut04
		//Then heavily simplified for general use

		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE resource;

		// Lock the constant buffer so it can be written to.
		hr = m_pDeviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		if (FAILED(hr))
		{
			return;
		}

		// Copy data to the buffer
		memcpy(resource.pData, data, dataSize);

		// Unlock the constant buffer.
		m_pDeviceContext->Unmap(buffer, 0);

		// Finanly set the constant buffer in the vertex shader with the updated values.
		switch (sType)
		{
		case ShaderType::Vertex:
			m_pDeviceContext->VSSetConstantBuffers(bufferIndex, 1, &buffer);
			break;
		case ShaderType::Pixel:
			m_pDeviceContext->PSSetConstantBuffers(bufferIndex, 1, &buffer);
			break;
		case ShaderType::Compute:
			m_pDeviceContext->CSSetConstantBuffers(bufferIndex, 1, &buffer);
			break;
		}
	}
}