#include "DXRenderDevice.h"

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
		if(m_pManager != nullptr) delete m_pManager;

		// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
		if (m_pSwapChain)
		{
			m_pSwapChain->SetFullscreenState(false, NULL);
		}

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
		descDepth.Width = m_ScreenWidth;
		descDepth.Height = m_ScreenHeight;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT;
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
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &descDSV, &m_pDepthStencilView);
		if (FAILED(hr)) return false;

		// Select the back buffer and depth buffer to use for rendering now
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		// Setup the raster description which will determine how and what polygons will be drawn.
		D3D11_RASTERIZER_DESC descRaster;
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
		vp.Width = m_ScreenWidth;
		vp.Height = m_ScreenHeight;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_pDeviceContext->RSSetViewports(1, &vp);

		return true;
	}


	///////////////////////////
	// Rendering

	//Renders the scene
	void DXRenderDevice::RenderScene()
	{

	}
}