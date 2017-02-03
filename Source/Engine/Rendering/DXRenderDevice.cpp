#include "Rendering\DXRenderDevice.h"
#include <DirectXMath.h>

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

		if (m_pModelShader != nullptr) delete m_pModelShader;
		if (m_pModelShader != nullptr) delete m_pDepthShader;
		if (m_pLightCullCS != nullptr) delete m_pLightCullCS;

		// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
		if (m_pSwapChain)
		{
			m_pSwapChain->SetFullscreenState(false, NULL);
		}

		if (m_GlobalMatrixConstBuffer != nullptr) delete m_GlobalMatrixConstBuffer;
		if (m_ObjMatrixConstBuffer != nullptr) delete m_ObjMatrixConstBuffer;
		if (m_GlobalLightConstBuffer != nullptr) delete m_GlobalLightConstBuffer;
		if (m_MaterialConstBuffer != nullptr) delete m_MaterialConstBuffer;
		if (m_GlobalThreadConstBuffer != nullptr) delete m_GlobalThreadConstBuffer;
		if (m_pLightStructuredBuffer != nullptr) delete m_pLightStructuredBuffer;
		if (m_pFrustumStructuredBuffer != nullptr) delete m_pFrustumStructuredBuffer;
		if (m_pLightIndexStructuredBuffer != nullptr) delete m_pLightIndexStructuredBuffer;
		if (m_pLightOffsetStructuredBuffer != nullptr) delete m_pLightOffsetStructuredBuffer;
		if (m_pLightGrid != nullptr) delete m_pLightGrid;

		SAFE_RELEASE(m_pSamplerState);
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

		m_pLightCullCS = new DXG::Shader;
		if (!m_pLightCullCS->Init(m_pDevice, DXG::ShaderType::Compute, ".\\LightCull.cso"))
		{
			return false;
		}

		m_ObjMatrixConstBuffer = new ConstBuffer<ObjectMatrix>;
		m_GlobalMatrixConstBuffer = new ConstBuffer<GlobalMatrix>;
		m_GlobalLightConstBuffer = new ConstBuffer<GlobalLightData>;
		m_MaterialConstBuffer = new ConstBuffer<MaterialData>;
		m_GlobalThreadConstBuffer = new ConstBuffer<GlobalThreadData>;
		m_pLightStructuredBuffer = new DXG::StructuredBuffer<Light>;
		m_pLightIndexStructuredBuffer = new DXG::StructuredBuffer<DXG::uint>;
		m_pLightOffsetStructuredBuffer = new DXG::StructuredBuffer<DXG::uint>;
		m_pLightGrid = new Texture2D;

		if (!m_ObjMatrixConstBuffer->Init(m_pDevice) ||
			!m_GlobalMatrixConstBuffer->Init(m_pDevice) ||
			!m_GlobalLightConstBuffer->Init(m_pDevice) ||
			!m_MaterialConstBuffer->Init(m_pDevice) ||
			!m_GlobalThreadConstBuffer->Init(m_pDevice) ||
			!m_pLightStructuredBuffer->Init(m_pDevice, Scene::kMaxLights, DXG::CPUAccess::Write) ||
			!m_pLightIndexStructuredBuffer->Init(m_pDevice, ((m_ScreenHeight + 15) / 16) * ((m_ScreenWidth + 15) / 16) * 256, DXG::CPUAccess::None, true) ||
			!m_pLightOffsetStructuredBuffer->Init(m_pDevice, 4, DXG::CPUAccess::None, true, true) ||
			!m_pLightGrid->Init(m_pDevice, (m_ScreenWidth + 15) / 16, (m_ScreenHeight + 15) / 16))
		{
			return false;
		}
		else
		{
			GlobalLightData& data = m_GlobalLightConstBuffer->GetMutable();
			data.AmbientColour = { 0.1f, 0.1f, 0.1f, 1.0f };
			data.SpecularPower = 64;

			//GlobalThreadData& data2 = m_GlobalThreadConstBuffer->GetMutable();
			
			m_GlobalThreadConstBuffer->Set({ { 16,16,1,0 },{ (m_ScreenWidth + 15) / 16 , (m_ScreenHeight + 15) / 16, 1, 0 } });
		}

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
		// Pre Render

		GlobalMatrix& globalMatrix = m_GlobalMatrixConstBuffer->GetMutable();
		globalMatrix.ViewMatrix = m_pSceneManager->GetActiveCamera()->GetViewMatrix();
		globalMatrix.ProjMatrix = CalcPerspectiveMatrix(m_pSceneManager->GetActiveCamera());
		m_GlobalMatrixConstBuffer->Bind(m_pDeviceContext, DXG::ShaderType::Vertex, 0);

		int numOfLights = 0;
		for (auto light : m_pSceneManager->m_LightList)
		{
			Light& lightData = (*m_pLightStructuredBuffer)[numOfLights];
			lightData.Brightness = light->GetBrightness();
			lightData.Position = light->WorldMatrix().Position();
			lightData.Colour = light->GetColour();
			lightData.Range = light->GetBrightness();
			++numOfLights;
		}
		m_pLightStructuredBuffer->SetDirty();

		///////////////////////////
		// Depth pre pass

		/*m_pDepthShader->SetTechnique(m_pDeviceContext);

		for (auto itr = m_pSceneManager->m_ModelMap.begin(); itr != m_pSceneManager->m_ModelMap.end(); ++itr)
		{
			(*itr).first->SetBuffers(m_pDeviceContext);
			auto& modelList = (*itr).second;

			for (auto modelItr = modelList.begin(); modelItr != modelList.end(); ++modelItr)
			{
				g_ObjMatrix.world = (*modelItr)->Matrix();
				SetConstantBuffer(m_pObjMatrixBuffer, 1, &g_ObjMatrix, sizeof(ObjectMatrixBuffer), DXG::ShaderType::Vertex);
				m_pDeviceContext->DrawIndexed((*itr).first->GetIndexCount(), 0, 0);
			}
		}*/


		///////////////////////////
		// Lighting compute



		ID3D11UnorderedAccessView* uavViews[1];
		uavViews[0] = m_pLightOffsetStructuredBuffer->GetUnorderedAccessView();
		//m_pDeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, 0, 0, nullptr, nullptr);
		//m_pDeviceContext->OMSetRenderTargetsAndUnorderedAccessViews(1, &m_pRenderTargetView, m_pDepthStencilView, 2, 1, uavViews, nullptr);
		//m_pLightOffsetStructuredBuffer->Clear(m_pDeviceContext);
		DXG::uint initCounts[] = {0,0,0,0};

		m_pLightCullCS->SetShader(m_pDeviceContext);
		m_GlobalThreadConstBuffer->Bind(m_pDeviceContext, DXG::ShaderType::Compute, 0);
		m_GlobalLightConstBuffer->Bind(m_pDeviceContext, DXG::ShaderType::Compute, 1);
		m_pLightStructuredBuffer->Bind(m_pDeviceContext, DXG::ShaderType::Compute, 0);
		m_pLightIndexStructuredBuffer->Bind(m_pDeviceContext, DXG::ShaderType::Compute, 0, DXG::BufferType::UAV);
		m_pLightGrid->Bind(m_pDeviceContext, DXG::ShaderType::Compute, 1, DXG::BufferType::UAV);
		m_pLightOffsetStructuredBuffer->Bind(m_pDeviceContext, DXG::ShaderType::Compute, 2, DXG::BufferType::UAV, initCounts);

		m_pDeviceContext->Dispatch((m_ScreenWidth + 15) / 16, (m_ScreenHeight + 15) / 16,1);

		m_GlobalThreadConstBuffer->Unbind(m_pDeviceContext, DXG::ShaderType::Compute, 0);
		m_GlobalLightConstBuffer->Unbind(m_pDeviceContext, DXG::ShaderType::Compute, 1);
		m_pLightStructuredBuffer->Unbind(m_pDeviceContext, DXG::ShaderType::Compute, 0);
		m_pLightIndexStructuredBuffer->Unbind(m_pDeviceContext, DXG::ShaderType::Compute, 0, DXG::BufferType::UAV);
		m_pLightGrid->Unbind(m_pDeviceContext, DXG::ShaderType::Compute, 1, DXG::BufferType::UAV);
		m_pLightOffsetStructuredBuffer->Unbind(m_pDeviceContext, DXG::ShaderType::Compute, 2, DXG::BufferType::UAV);
		

		///////////////////////////
		// Lighting pass

		m_pModelShader->SetShader(m_pDeviceContext);

		GlobalLightData& globalLightData = m_GlobalLightConstBuffer->GetMutable();
		globalLightData.CameraPos = gen::CVector4(m_pSceneManager->GetActiveCamera()->WorldMatrix().Position());
		globalLightData.NumOfLights = numOfLights;
		globalLightData.ScreenWidth = m_ScreenWidth;
		globalLightData.ScreenHeight = m_ScreenHeight;


		m_GlobalMatrixConstBuffer->Bind(m_pDeviceContext, DXG::ShaderType::Vertex, 0);
		m_ObjMatrixConstBuffer->Bind(m_pDeviceContext, DXG::ShaderType::Vertex, 1);

		m_GlobalLightConstBuffer->Bind(m_pDeviceContext, DXG::ShaderType::Pixel, 0);
		m_MaterialConstBuffer->Bind(m_pDeviceContext, DXG::ShaderType::Pixel, 1);
		m_pLightStructuredBuffer->Bind(m_pDeviceContext, DXG::ShaderType::Pixel, 2);
		m_pLightIndexStructuredBuffer->Bind(m_pDeviceContext, DXG::ShaderType::Pixel, 3);
		m_pLightGrid->Bind(m_pDeviceContext, DXG::ShaderType::Pixel, 4);

		for (auto itr = m_pSceneManager->m_ModelMap.begin(); itr != m_pSceneManager->m_ModelMap.end(); ++itr)
		{
			(*itr).first->SetBuffers(m_pDeviceContext);
			auto& modelList = (*itr).second;

			for (auto modelItr = modelList.begin(); modelItr != modelList.end(); ++modelItr)
			{
				m_ObjMatrixConstBuffer->Set({ (*modelItr)->WorldMatrix() });
				m_ObjMatrixConstBuffer->CommitChanges(m_pDeviceContext);

				Material* pMat = (*modelItr)->GetMaterial();
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

				m_pDeviceContext->DrawIndexed((*itr).first->GetIndexCount(), 0, 0);
			}
		}

		m_GlobalMatrixConstBuffer->Unbind(m_pDeviceContext, DXG::ShaderType::Vertex, 0);
		m_ObjMatrixConstBuffer->Unbind(m_pDeviceContext, DXG::ShaderType::Vertex, 1);

		m_GlobalLightConstBuffer->Unbind(m_pDeviceContext, DXG::ShaderType::Pixel, 0);
		m_MaterialConstBuffer->Unbind(m_pDeviceContext, DXG::ShaderType::Pixel, 1);
		m_pLightStructuredBuffer->Unbind(m_pDeviceContext, DXG::ShaderType::Pixel, 2);
		m_pLightIndexStructuredBuffer->Unbind(m_pDeviceContext, DXG::ShaderType::Pixel, 3);
		m_pLightGrid->Unbind(m_pDeviceContext, DXG::ShaderType::Pixel, 4);

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

		//DirectX::XMMATRIX mat = DirectX::XMMatrixPerspectiveFovLH( DirectX::XMConvertToRadians(45.0f), hbyw, 50.0f, 2000.0f);
		DirectX::XMMATRIX mat = DirectX::XMMatrixPerspectiveFovLH(gen::ToRadians(camera->GetFOV()), hbyw * 1.5f, camera->GetNearClip(), camera->GetFarClip());
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