#include "Rendering\DXRenderDevice.h"
#include <DirectXMath.h>
#include "Input.h"

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
		if (m_pSceneManager != nullptr) delete m_pSceneManager;
		if (m_pMeshManager != nullptr) delete m_pMeshManager;
		if (m_pMaterialManager != nullptr) delete m_pMaterialManager;
		if (m_pTextureManager != nullptr) delete m_pTextureManager;

		if (m_pDepthVS != nullptr) delete m_pDepthVS;
		if (m_pDepthPS != nullptr) delete m_pDepthPS;
		if (m_pModelVS != nullptr) delete m_pModelVS;
		if (m_pModelPS != nullptr) delete m_pModelPS;
		if (m_pLightCullCS != nullptr) delete m_pLightCullCS;
		if (m_pCopyCS != nullptr) delete m_pCopyCS;
		if (m_pFrustumCalcCS != nullptr) delete m_pFrustumCalcCS;
		if (m_pHeatMapVS != nullptr) delete m_pHeatMapVS;
		if (m_pHeatMapPS != nullptr) delete m_pHeatMapPS;
		if (m_pForwardPS != nullptr) delete m_pForwardPS;

		// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
		if (m_pSwapChain)
		{
			m_pSwapChain->SetFullscreenState(false, NULL);
		}

		//Const buffers
		if (m_GlobalMatrixConstBuffer != nullptr) delete m_GlobalMatrixConstBuffer;
		if (m_ObjMatrixConstBuffer != nullptr) delete m_ObjMatrixConstBuffer;
		if (m_GlobalLightConstBuffer != nullptr) delete m_GlobalLightConstBuffer;
		if (m_MaterialConstBuffer != nullptr) delete m_MaterialConstBuffer;
		if (m_GlobalThreadConstBuffer != nullptr) delete m_GlobalThreadConstBuffer;
		if (m_BufferCopyConstBuffer != nullptr) delete m_BufferCopyConstBuffer;
		if (m_FrustumConstBuffer != nullptr) delete m_FrustumConstBuffer;

		//Structured buffers
		if (m_pLightStructuredBuffer != nullptr) delete m_pLightStructuredBuffer;
		if (m_pFrustumStructuredBuffer != nullptr) delete m_pFrustumStructuredBuffer;
		if (m_pLightIndexStructuredBuffer != nullptr) delete m_pLightIndexStructuredBuffer;
		if (m_pLightOffsetStructuredBuffer != nullptr) delete m_pLightOffsetStructuredBuffer;
		if (m_pZeroedStructuredBuffer != nullptr) delete m_pZeroedStructuredBuffer;

		//2D Textures
		if (m_pLightGrid != nullptr) delete m_pLightGrid;

		SAFE_RELEASE(m_pSamplerState);
		SAFE_RELEASE(m_pRasterState);
		SAFE_RELEASE(m_pDepthStencilView);
		SAFE_RELEASE(m_pDepthStencilState);
		SAFE_RELEASE(m_pDepthStencilBuffer);
		SAFE_RELEASE(m_pDepthResourceView);
		SAFE_RELEASE(m_pDepthTexture);
		SAFE_RELEASE(m_pDepthRenderTargetView);
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
		m_TileRows = ((m_ScreenHeight + 15) / 16);
		m_TileCols = ((m_ScreenWidth + 15) / 16);

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
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.OutputWindow = hWnd;                            // Target window
		sd.Windowed = TRUE;                                // Whether to render in a window (TRUE) or go fullscreen (FALSE)
		hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &featureLevel, 1,
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
		descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;
		hr = m_pDevice->CreateTexture2D(&descDepth, NULL, &m_pDepthStencilBuffer);
		if (FAILED(hr)) return false;


		// Depth texture
		descDepth.Format = DXGI_FORMAT_R32_FLOAT;
		descDepth.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D10_BIND_RENDER_TARGET;
		hr = m_pDevice->CreateTexture2D(&descDepth, NULL, &m_pDepthTexture);
		if (FAILED(hr)) return false;

		hr = m_pDevice->CreateRenderTargetView(m_pDepthTexture, NULL, &m_pDepthRenderTargetView);
		if (FAILED(hr)) return false;

		// Create the depth shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC descDepthSRV;
		ZeroMemory(&descDepthSRV, sizeof(descDepthSRV));
		descDepthSRV.Format = DXGI_FORMAT_R32_FLOAT;
		descDepthSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		descDepthSRV.Texture2D.MostDetailedMip = 0;
		descDepthSRV.Texture2D.MipLevels = -1;

		hr = m_pDevice->CreateShaderResourceView(m_pDepthTexture, &descDepthSRV, &m_pDepthResourceView);
		if (FAILED(hr)) return false;


		// Create the depth stencil state
		D3D11_DEPTH_STENCIL_DESC descDSS;
		ZeroMemory(&descDSS, sizeof(descDSS));

		// Set up the description of the stencil state.
		descDSS.DepthEnable = true;
		descDSS.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		descDSS.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // Need to be less or equal due to depth prepass

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
		if (FAILED(hr)) return false;

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

		D3D11_SAMPLER_DESC descSampler;
		ZeroMemory(&descSampler, sizeof(descSampler));
		descSampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		descSampler.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;    // Wrap texture addressing mode
		descSampler.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;    // --"--
		descSampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;    // --"--
		descSampler.ComparisonFunc = D3D11_COMPARISON_NEVER;  // Allows a test on the value sampled from texture prior to use (new to DX10 - not using here)
		descSampler.MinLOD = 0;                               // Set range of mip-maps to use, these values indicate...
		descSampler.MaxLOD = D3D11_FLOAT32_MAX;               // ...to use all available mip-maps (i.e. enable mip-mapping)
		m_pDevice->CreateSamplerState(&descSampler, &m_pSamplerState);
		m_pDeviceContext->PSSetSamplers(0, 1, &m_pSamplerState);

		m_pDepthVS = new DXG::Shader;
		if (!m_pDepthVS->Init(m_pDevice, DXG::ShaderType::Vertex, ".\\DepthVS.cso"))
		{
			return false;
		}

		m_pDepthPS = new DXG::Shader;
		if (!m_pDepthPS->Init(m_pDevice, DXG::ShaderType::Pixel, ".\\DepthPS.cso"))
		{
			return false;
		}

		m_pModelVS = new DXG::Shader;
		if (!m_pModelVS->Init(m_pDevice, DXG::ShaderType::Vertex, ".\\ModelVS.cso"))
		{
			return false;
		}

		m_pModelPS = new DXG::Shader;
		if (!m_pModelPS->Init(m_pDevice, DXG::ShaderType::Pixel, ".\\ModelPS.cso"))
		{
			return false;
		}

		m_pLightCullCS = new DXG::Shader;
		if (!m_pLightCullCS->Init(m_pDevice, DXG::ShaderType::Compute, ".\\LightCull.cso"))
		{
			return false;
		}

		m_pCopyCS = new DXG::Shader;
		if (!m_pCopyCS->Init(m_pDevice, DXG::ShaderType::Compute, ".\\CopyBufferCS.cso"))
		{
			return false;
		}

		m_pFrustumCalcCS = new DXG::Shader;
		if (!m_pFrustumCalcCS->Init(m_pDevice, DXG::ShaderType::Compute, ".\\FrustumCalc.cso"))
		{
			return false;
		}
		m_pHeatMapVS = new DXG::Shader;
		if (!m_pHeatMapVS->Init(m_pDevice, DXG::ShaderType::Vertex, ".\\HeatMapVS.cso"))
		{
			return false;
		}

		m_pHeatMapPS = new DXG::Shader;
		if (!m_pHeatMapPS->Init(m_pDevice, DXG::ShaderType::Pixel, ".\\HeatMapPS.cso"))
		{
			return false;
		}

		m_pForwardPS = new DXG::Shader;
		if (!m_pForwardPS->Init(m_pDevice, DXG::ShaderType::Pixel, ".\\ForwardPS.cso"))
		{
			return false;
		}

		m_ObjMatrixConstBuffer = new ConstBuffer<ObjectMatrix>;
		m_GlobalMatrixConstBuffer = new ConstBuffer<GlobalMatrix>;
		m_GlobalLightConstBuffer = new ConstBuffer<GlobalLightData>;
		m_MaterialConstBuffer = new ConstBuffer<MaterialData>;
		m_GlobalThreadConstBuffer = new ConstBuffer<GlobalThreadData>;
		m_BufferCopyConstBuffer = new ConstBuffer<CopyDetails>;
		m_FrustumConstBuffer = new ConstBuffer<FrustumData>;

		m_pLightStructuredBuffer = new DXG::StructuredBuffer<Light>;
		m_pFrustumStructuredBuffer = new DXG::StructuredBuffer<Frustum>;
		m_pLightIndexStructuredBuffer = new DXG::StructuredBuffer<DXG::uint>;
		m_pLightOffsetStructuredBuffer = new DXG::StructuredBuffer<DXG::uint>;
		m_pZeroedStructuredBuffer = new DXG::StructuredBuffer<DXG::uint>;
		m_pLightGrid = new Texture2D;

		if (!m_ObjMatrixConstBuffer->Init(m_pDevice) ||
			!m_GlobalMatrixConstBuffer->Init(m_pDevice) ||
			!m_GlobalLightConstBuffer->Init(m_pDevice) ||
			!m_MaterialConstBuffer->Init(m_pDevice) ||
			!m_GlobalThreadConstBuffer->Init(m_pDevice) ||
			!m_BufferCopyConstBuffer->Init(m_pDevice) ||
			!m_FrustumConstBuffer->Init(m_pDevice))
		{
			return false;
		}
		else if (!m_pLightStructuredBuffer->Init(m_pDevice, Scene::kMaxLights, DXG::CPUAccess::Write) ||
			!m_pLightIndexStructuredBuffer->Init(m_pDevice, m_TileRows * m_TileCols * 256, DXG::CPUAccess::None, true) ||
			!m_pFrustumStructuredBuffer->Init(m_pDevice, m_TileRows * m_TileCols, DXG::CPUAccess::None, true) ||
			!m_pLightOffsetStructuredBuffer->Init(m_pDevice, 16, DXG::CPUAccess::None, true) ||
			!m_pZeroedStructuredBuffer->Init(m_pDevice, 16, DXG::CPUAccess::Write, false) ||
			!m_pLightGrid->Init(m_pDevice, (m_ScreenWidth + 15) / 16, (m_ScreenHeight + 15) / 16))
		{
			return false;
		}
		else
		{
			GlobalLightData& data = m_GlobalLightConstBuffer->GetMutable();
			data.AmbientColour = { 0.1f, 0.1f, 0.1f, 1.0f };
			data.SpecularPower = 64;

			auto& copyDetails = m_BufferCopyConstBuffer->GetMutable();
			copyDetails.DestinationSize = 16;
			copyDetails.SourceSize = 16;
			copyDetails.MinSize = 16;

			for(int i = 0; i < 4; ++i)
				m_pZeroedStructuredBuffer->Set(i, 0);
			
			m_GlobalThreadConstBuffer->Set({ { 16,16,1,0 },{ (m_ScreenWidth + 15) / 16 , (m_ScreenHeight + 15) / 16, 1, 0 } });
		}

		//Copy pass
		m_CopyPass.AddShader(m_pCopyCS);
		m_CopyPass.AddResource(m_GlobalThreadConstBuffer,			DXG::ShaderType::Compute, 0, DXG::BufferType::Constant);
		m_CopyPass.AddResource(m_BufferCopyConstBuffer,				DXG::ShaderType::Compute, 1, DXG::BufferType::Constant);
		m_CopyPass.AddResource(m_pZeroedStructuredBuffer,			DXG::ShaderType::Compute, 0, DXG::BufferType::Structured);
		m_CopyPass.AddResource(m_pLightOffsetStructuredBuffer,		DXG::ShaderType::Compute, 0, DXG::BufferType::UAV);

		//Full render pass
		m_FullRenderPass.AddShader(m_pModelVS);
		m_FullRenderPass.AddShader(m_pModelPS);
		m_FullRenderPass.AddResource(m_GlobalMatrixConstBuffer,		DXG::ShaderType::Vertex, 0, DXG::BufferType::Constant);
		m_FullRenderPass.AddResource(m_ObjMatrixConstBuffer,		DXG::ShaderType::Vertex, 1, DXG::BufferType::Constant);
		m_FullRenderPass.AddResource(m_GlobalLightConstBuffer,		DXG::ShaderType::Pixel,  0, DXG::BufferType::Constant);
		m_FullRenderPass.AddResource(m_MaterialConstBuffer,			DXG::ShaderType::Pixel,  1, DXG::BufferType::Constant);
		m_FullRenderPass.AddResource(m_pLightStructuredBuffer,		DXG::ShaderType::Pixel,  2, DXG::BufferType::Structured);
		m_FullRenderPass.AddResource(m_pLightIndexStructuredBuffer, DXG::ShaderType::Pixel,  3, DXG::BufferType::Structured);
		m_FullRenderPass.AddResource(m_pLightGrid,					DXG::ShaderType::Pixel,  4, DXG::BufferType::Structured);
		
		//Light cull pass
		m_LightCullPass.AddShader(m_pLightCullCS);
		m_LightCullPass.AddResource(m_GlobalThreadConstBuffer,		DXG::ShaderType::Compute, 0, DXG::BufferType::Constant);
		m_LightCullPass.AddResource(m_GlobalLightConstBuffer,		DXG::ShaderType::Compute, 1, DXG::BufferType::Constant);
		m_LightCullPass.AddResource(m_pLightStructuredBuffer,		DXG::ShaderType::Compute, 0, DXG::BufferType::Structured);
		m_LightCullPass.AddResource(m_pFrustumStructuredBuffer,		DXG::ShaderType::Compute, 1, DXG::BufferType::Structured);
		m_LightCullPass.AddResource(m_pLightIndexStructuredBuffer,	DXG::ShaderType::Compute, 0, DXG::BufferType::UAV);
		m_LightCullPass.AddResource(m_pLightGrid,					DXG::ShaderType::Compute, 1, DXG::BufferType::UAV);
		m_LightCullPass.AddResource(m_pLightOffsetStructuredBuffer, DXG::ShaderType::Compute, 2, DXG::BufferType::UAV);

		//Frustum calc pass
		m_FrustumPass.AddShader(m_pFrustumCalcCS);
		m_FrustumPass.AddResource(m_GlobalThreadConstBuffer,		DXG::ShaderType::Compute, 0, DXG::BufferType::Constant);
		m_FrustumPass.AddResource(m_FrustumConstBuffer,				DXG::ShaderType::Compute, 1, DXG::BufferType::Constant);
		m_FrustumPass.AddResource(m_GlobalMatrixConstBuffer,		DXG::ShaderType::Compute, 2, DXG::BufferType::Constant);
		m_FrustumPass.AddResource(m_pFrustumStructuredBuffer,		DXG::ShaderType::Compute, 0, DXG::BufferType::UAV);

		//Depth pre pass
		m_DepthPass.AddShader(m_pDepthVS);
		m_DepthPass.AddShader(m_pDepthPS);
		m_DepthPass.AddResource(m_GlobalMatrixConstBuffer,			DXG::ShaderType::Vertex, 0, DXG::BufferType::Constant);
		m_DepthPass.AddResource(m_ObjMatrixConstBuffer,				DXG::ShaderType::Vertex, 1, DXG::BufferType::Constant);
		m_DepthPass.AddResource(m_FrustumConstBuffer,				DXG::ShaderType::Pixel,  0, DXG::BufferType::Constant);

		//Heat map pass
		m_HeatMapPass.AddShader(m_pHeatMapVS);
		m_HeatMapPass.AddShader(m_pHeatMapPS);
		m_HeatMapPass.AddResource(m_GlobalMatrixConstBuffer,		DXG::ShaderType::Vertex, 0, DXG::BufferType::Constant);
		m_HeatMapPass.AddResource(m_ObjMatrixConstBuffer,			DXG::ShaderType::Vertex, 1, DXG::BufferType::Constant);
		m_HeatMapPass.AddResource(m_pLightGrid,						DXG::ShaderType::Pixel,  0, DXG::BufferType::Structured);

		//Forward rendering
		m_ForwardPass.AddShader(m_pModelVS);
		m_ForwardPass.AddShader(m_pForwardPS);
		m_ForwardPass.AddResource(m_GlobalMatrixConstBuffer,		DXG::ShaderType::Vertex, 0, DXG::BufferType::Constant);
		m_ForwardPass.AddResource(m_ObjMatrixConstBuffer,			DXG::ShaderType::Vertex, 1, DXG::BufferType::Constant);
		m_ForwardPass.AddResource(m_GlobalLightConstBuffer,			DXG::ShaderType::Pixel,  0, DXG::BufferType::Constant);
		m_ForwardPass.AddResource(m_MaterialConstBuffer,			DXG::ShaderType::Pixel,  1, DXG::BufferType::Constant);
		m_ForwardPass.AddResource(m_pLightStructuredBuffer,			DXG::ShaderType::Pixel,  2, DXG::BufferType::Structured);

		m_pMeshManager = new MeshManager(m_pDevice);
		m_pSceneManager = new Scene::Manager(m_pMeshManager);
		m_pTextureManager = new TextureManager(m_pDevice);
		m_pMaterialManager = new MaterialManager(m_pTextureManager);

		return true;
	}


	///////////////////////////
	// Rendering

	//Renders the scene
	void DXRenderDevice::RenderScene()
	{
		ClearScreen();

		///////////////////////////
		// Pre Render Data Gather

		Scene::Camera* activeCamera = m_pSceneManager->GetActiveCamera();

		GlobalMatrix& globalMatrix = m_GlobalMatrixConstBuffer->GetMutable();
		globalMatrix.ViewMatrix = activeCamera->GetViewMatrix();
		globalMatrix.ProjMatrix = CalcPerspectiveMatrix(activeCamera);
		globalMatrix.InvProjMatrix = gen::Inverse(globalMatrix.ProjMatrix);
		m_GlobalMatrixConstBuffer->Bind(m_pDeviceContext, DXG::ShaderType::Vertex, 0, DXG::BufferType::Constant);

		int numOfLights = 0;
		for (auto light : m_pSceneManager->m_LightList)
		{
			Light& lightData = (*m_pLightStructuredBuffer)[numOfLights];
			lightData.Brightness = light->GetBrightness();
			lightData.Position = light->WorldMatrix().Position();
			lightData.Colour = light->GetColour();
			lightData.Range = light->GetRange();
			++numOfLights;
		}
		m_pLightStructuredBuffer->SetDirty();

		GlobalLightData& globalLightData = m_GlobalLightConstBuffer->GetMutable();
		globalLightData.CameraPos = gen::CVector4(m_pSceneManager->GetActiveCamera()->WorldMatrix().Position());
		globalLightData.NumOfLights = numOfLights;
		globalLightData.ScreenWidth = static_cast<float>(m_ScreenWidth);
		globalLightData.ScreenHeight = static_cast<float>(m_ScreenHeight);

		FrustumData& frustumData = m_FrustumConstBuffer->GetMutable();
		frustumData.CameraRight = activeCamera->Matrix().GetRow(0);
		frustumData.CameraUp = activeCamera->Matrix().GetRow(1);
		frustumData.CameraForward = activeCamera->Matrix().GetRow(2);
		frustumData.CameraPos = activeCamera->Matrix().GetRow(3);
		frustumData.FarDistance = activeCamera->GetFarClip();
		frustumData.NearDistance = activeCamera->GetNearClip();
		frustumData.FOV = activeCamera->GetFOV();
		frustumData.NumTileCols = m_TileCols;
		frustumData.NumTileRows = m_TileRows;
		frustumData.Ratio = static_cast<float>(m_ScreenWidth) / static_cast<float>(m_ScreenHeight);
		frustumData.ScreenWidth = static_cast<float>(m_ScreenWidth);
		frustumData.ScreenHeight = static_cast<float>(m_ScreenHeight);
		frustumData.CameraMatrix = activeCamera->Matrix();

		switch (m_RenderMethod)
		{
		case RenderMethod::Forward:
			RenderForward();
			break;
		case RenderMethod::ForwardPlus:
			RenderForwardPlus();
			break;
		}

		if (KeyHit(Key_O)) m_RenderMethod = RenderMethod::Forward;
		if (KeyHit(Key_P)) m_RenderMethod = RenderMethod::ForwardPlus;
	}

	//Forward rendering
	void DXRenderDevice::RenderForward()
	{
		ID3D11ShaderResourceView* clearResourceViews[] = { NULL, NULL };

		///////////////////////////
		// Model Render pass

		m_ForwardPass.Bind(m_pDeviceContext);

		//Ensure initial texture is null
		m_pDeviceContext->PSSetShaderResources(0, 2, clearResourceViews);

		for (auto itr = m_pSceneManager->m_ModelMap.begin(); itr != m_pSceneManager->m_ModelMap.end(); ++itr)
		{
			(*itr).first->SetBuffers(m_pDeviceContext);
			auto& modelList = (*itr).second;

			Material* pMat = nullptr;

			for (auto modelItr = modelList.begin(); modelItr != modelList.end(); ++modelItr)
			{
				m_ObjMatrixConstBuffer->Set({ (*modelItr)->WorldMatrix() });
				m_ObjMatrixConstBuffer->CommitChanges(m_pDeviceContext);

				if (pMat != (*modelItr)->GetMaterial())
				{
					pMat = (*modelItr)->GetMaterial();
					MaterialData& matData = m_MaterialConstBuffer->GetMutable();
					matData.DiffuseColour = pMat->GetDiffuseColour();
					matData.Alpha = pMat->GetAlpha();
					matData.Dirtyness = pMat->GetDirtyness();
					matData.Shinyness = pMat->GetShinyness();
					matData.HasAlpha = pMat->HasAlpha() ? 1 : 0;
					matData.HasDirt = pMat->HasDirt() ? 1 : 0;
					matData.HasDiffuseTex = pMat->HasDiffuseTex() ? 1 : 0;
					matData.HasSpecTex = pMat->HasSpecularTex() ? 1 : 0;
					m_MaterialConstBuffer->CommitChanges(m_pDeviceContext);

					if (pMat->HasDiffuseTex())
					{
						m_pDeviceContext->PSSetShaderResources(0, 1, pMat->GetDiffuseTexPtr());
					}
				}

				m_pDeviceContext->DrawIndexed((*itr).first->GetIndexCount(), 0, 0);
			}
		}
		//Unbind any texture files as they are not bound as apart of the render pass but instead per material
		m_pDeviceContext->PSSetShaderResources(0, 2, clearResourceViews);
		m_ForwardPass.Unbind(m_pDeviceContext);

		m_pSwapChain->Present(0, 0);
	}

	//Forward+ rendering
	void DXRenderDevice::RenderForwardPlus()
	{
		ID3D11ShaderResourceView* clearResourceViews[] = { NULL, NULL };

		///////////////////////////
		// Depth pre pass

		m_DepthPass.Bind(m_pDeviceContext);

		m_pDeviceContext->OMSetRenderTargets(1, &m_pDepthRenderTargetView, m_pDepthStencilView);
		for (auto itr = m_pSceneManager->m_ModelMap.begin(); itr != m_pSceneManager->m_ModelMap.end(); ++itr)
		{
			(*itr).first->SetBuffers(m_pDeviceContext);
			auto& modelList = (*itr).second;

			for (auto modelItr = modelList.begin(); modelItr != modelList.end(); ++modelItr)
			{
				m_ObjMatrixConstBuffer->Set({ (*modelItr)->WorldMatrix() });
				m_ObjMatrixConstBuffer->CommitChanges(m_pDeviceContext);

				m_pDeviceContext->DrawIndexed((*itr).first->GetIndexCount(), 0, 0);
			}
		}
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

		m_DepthPass.Unbind(m_pDeviceContext);

		///////////////////////////
		// Copy reset

		//set buffer data
		m_GlobalThreadConstBuffer->Set({ { 2, 2, 1, 0 }, { 1, 1, 1, 0 } });

		//dispatch
		m_CopyPass.Bind(m_pDeviceContext);
		m_pDeviceContext->Dispatch(1, 1, 1);
		m_CopyPass.Unbind(m_pDeviceContext);

		///////////////////////////
		// Frustum calc

		//set buffer data
		m_GlobalThreadConstBuffer->Set({ { 16, 16, 1, 0 }, { (m_TileCols + 15) / 16, (m_TileRows + 15) /16, 1, 0 } });

		//dispatch
		m_FrustumPass.Bind(m_pDeviceContext);
		m_pDeviceContext->Dispatch((m_TileCols + 15) / 16, (m_TileRows + 15) / 16, 1);
		m_FrustumPass.Unbind(m_pDeviceContext);

		///////////////////////////
		// Lighting compute

		//set buffer data
		m_GlobalThreadConstBuffer->Set({ { 16, 16, 1, 0 }, { m_TileCols , m_TileRows, 1, 0 } });

		//dispatch
		m_LightCullPass.Bind(m_pDeviceContext);
		m_pDeviceContext->CSSetShaderResources(2, 1, &m_pDepthResourceView);
		m_pDeviceContext->Dispatch((m_ScreenWidth + 15) / 16, (m_ScreenHeight + 15) / 16,1);
		m_pDeviceContext->CSSetShaderResources(2, 1, clearResourceViews);
		m_LightCullPass.Unbind(m_pDeviceContext);

		///////////////////////////
		// Model Render pass

		m_FullRenderPass.Bind(m_pDeviceContext);

		//Ensure initial texture is null
		m_pDeviceContext->PSSetShaderResources(0, 2, clearResourceViews);

		for (auto itr = m_pSceneManager->m_ModelMap.begin(); itr != m_pSceneManager->m_ModelMap.end(); ++itr)
		{
			(*itr).first->SetBuffers(m_pDeviceContext);
			auto& modelList = (*itr).second;

			Material* pMat = nullptr;

			for (auto modelItr = modelList.begin(); modelItr != modelList.end(); ++modelItr)
			{
				m_ObjMatrixConstBuffer->Set({ (*modelItr)->WorldMatrix() });
				m_ObjMatrixConstBuffer->CommitChanges(m_pDeviceContext);

				if (pMat != (*modelItr)->GetMaterial())
				{
					pMat = (*modelItr)->GetMaterial();
					MaterialData& matData = m_MaterialConstBuffer->GetMutable();
					matData.DiffuseColour = pMat->GetDiffuseColour();
					matData.Alpha = pMat->GetAlpha();
					matData.Dirtyness = pMat->GetDirtyness();
					matData.Shinyness = pMat->GetShinyness();
					matData.HasAlpha = pMat->HasAlpha() ? 1 : 0;
					matData.HasDirt = pMat->HasDirt() ? 1 : 0;
					matData.HasDiffuseTex = pMat->HasDiffuseTex() ? 1 : 0;
					matData.HasSpecTex = pMat->HasSpecularTex() ? 1 : 0;
					m_MaterialConstBuffer->CommitChanges(m_pDeviceContext);

					if (pMat->HasDiffuseTex())
					{
						m_pDeviceContext->PSSetShaderResources(0, 1, pMat->GetDiffuseTexPtr());
					}
				}

				m_pDeviceContext->DrawIndexed((*itr).first->GetIndexCount(), 0, 0);
			}
		}
		//Unbind any texture files as they are not bound as apart of the render pass but instead per material
		m_pDeviceContext->PSSetShaderResources(0, 2, clearResourceViews);
		m_FullRenderPass.Unbind(m_pDeviceContext);

		///////////////////////////
		// Heat Map pass
		if (KeyHeld(EKeyCode::Key_H))
		{
			m_HeatMapPass.Bind(m_pDeviceContext);
			m_pDeviceContext->IASetInputLayout(NULL);
			m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			m_pDeviceContext->Draw(4, 0);
			m_HeatMapPass.Unbind(m_pDeviceContext);
		}

		m_pSwapChain->Present(0, 0);
	}


	///////////////////////////
	// Render steps

	//Resets the back buffer and depth buffers
	void DXRenderDevice::ClearScreen()
	{
		float ClearColor[4] = { 0.1f, 0.1f, 0.15f, 1.0f };
		
		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, ClearColor);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		float ClearColorWhite[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		m_pDeviceContext->ClearRenderTargetView(m_pDepthRenderTargetView, ClearColorWhite);
	}

	//Creates and sets the perspective matrix from a camera
	gen::CMatrix4x4 DXRenderDevice::CalcPerspectiveMatrix(Scene::Camera* camera)
	{
		//g_GlobalMatrices.projection.MakeIdentity();

		////Formula taken from D3DXMatrixPerspectiveFovLH

		//float ratio = static_cast<float>(m_ScreenWidth) / static_cast<float>(m_ScreenHeight);
		//float xScale = 1.0f / gen::Tan(gen::ToRadians(camera->GetFOV()) * 0.25f);
		//float yScale = xScale / ratio;

		float hbyw = static_cast<float>(m_ScreenHeight) / static_cast<float>(m_ScreenWidth);
		float wbyh = static_cast<float>(m_ScreenWidth) / static_cast<float>(m_ScreenHeight);

		//DirectX::XMMATRIX mat = DirectX::XMMatrixPerspectiveFovLH( DirectX::XMConvertToRadians(45.0f), hbyw, 50.0f, 2000.0f);
		DirectX::XMMATRIX mat = DirectX::XMMatrixPerspectiveFovLH(gen::ToRadians(camera->GetFOV()), wbyh, camera->GetNearClip(), camera->GetFarClip());
		DirectX::XMFLOAT4X4 mat4x4;
		DirectX::XMStoreFloat4x4(&mat4x4, mat);
		gen::CMatrix4x4 matrix;
		matrix.Set(mat4x4.m[0]);
		return matrix;

		/*g_GlobalMatrices.projection.e00 = yScale;
		g_GlobalMatrices.projection.e11 = xScale;
		g_GlobalMatrices.projection.e22 = camera->GetFarClip() / (camera->GetFarClip() - camera->GetNearClip());
		g_GlobalMatrices.projection.e32 = (-camera->GetFarClip() - (-(camera->GetNearClip()))) / (camera->GetFarClip() - camera->GetNearClip());
		g_GlobalMatrices.projection.e23 = 1.0f;
		g_GlobalMatrices.projection.e33 = 0.0f;

		g_GlobalMatrices.projection.Transpose();*/
	}
}