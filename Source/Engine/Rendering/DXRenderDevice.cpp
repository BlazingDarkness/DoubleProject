#include "Rendering\DXRenderDevice.h"
#include <DirectXMath.h>

namespace Render
{

	//Updated once per frame
	struct AL16 GlobalMatrixBuffer
	{
		AL16 gen::CMatrix4x4 view;
		AL16 gen::CMatrix4x4 projection;
	} g_GlobalMatrices;

	//Updated once per object
	struct AL16 ObjectMatrixBuffer
	{
		AL16 gen::CMatrix4x4 world;
	} g_ObjMatrix;

	//Updated once per frame
	struct AL16 GlobalLightDataBuffer
	{
		AL16 gen::CVector4 ambientColour;
		AL16 gen::CVector4 cameraPos;
		AL4 float specularPower;
		AL4 unsigned int numOfLights;
		AL4 float padding[2];
	} g_GlobalLightData;

	//Updated once per material change
	struct AL16 MaterialBuffer
	{
		AL16 gen::CVector4 diffuseColour;
		AL4 float alpha;
		AL4 float dirtyness;
		AL4 float shinyness;
		AL4 unsigned int hasAlpha; //Use 1/0 for true/false
		AL4 unsigned int hasDirt; //Use 1/0 for true/false
		AL4 unsigned int hasDiffuseTex; //Use 1/0 for true/false
		AL4 unsigned int hasSpecularTex; //Use 1/0 for true/false
		AL4 float padding;
	} g_MaterialData;

	//Updated once per frame
	struct AL16 LightPosBuffer
	{
		AL4 gen::CVector3 position;
		AL4 float brightness;
	} g_LightPosData[Scene::kMaxLights];

	//Updated once per frame
	struct AL16 LightColourBuffer
	{
		AL4 gen::CVector3 color;
		AL4 float alpha;
	} g_LightColourData[Scene::kMaxLights];

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

		// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
		if (m_pSwapChain)
		{
			m_pSwapChain->SetFullscreenState(false, NULL);
		}

		SAFE_RELEASE(m_pGlobalLightDataBuffer);
		SAFE_RELEASE(m_pMaterialBuffer);
		SAFE_RELEASE(m_pGlobalMatrixBuffer);
		SAFE_RELEASE(m_pObjMatrixBuffer);

		SAFE_RELEASE(m_pLightPosView);
		SAFE_RELEASE(m_pLightColourView);
		SAFE_RELEASE(m_pLightPosBuffer);
		SAFE_RELEASE(m_pLightColourBuffer);

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


		D3D11_BUFFER_DESC cbDesc;
		ZeroMemory(&cbDesc, sizeof(cbDesc));
		cbDesc.ByteWidth = sizeof(GlobalMatrixBuffer); // Constant buffer data is packed into float4 data - must round up to size of float4 (16)
		cbDesc.Usage = D3D11_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPU is only going to write to the constants (not read them)
		cbDesc.MiscFlags = 0;
		if (FAILED(m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pGlobalMatrixBuffer)))
		{
			return false;
		}

		cbDesc.ByteWidth = sizeof(ObjectMatrixBuffer); // Constant buffer data is packed into float4 data - must round up to size of float4 (16)
		if (FAILED(m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pObjMatrixBuffer)))
		{
			return false;
		}

		cbDesc.ByteWidth = sizeof(GlobalLightDataBuffer); // Constant buffer data is packed into float4 data - must round up to size of float4 (16)
		if (FAILED(m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pGlobalLightDataBuffer)))
		{
			return false;
		}

		cbDesc.ByteWidth = sizeof(MaterialBuffer); // Constant buffer data is packed into float4 data - must round up to size of float4 (16)
		if (FAILED(m_pDevice->CreateBuffer(&cbDesc, NULL, &m_pMaterialBuffer)))
		{
			return false;
		}

		D3D11_BUFFER_DESC dbDesc;
		ZeroMemory(&dbDesc, sizeof(dbDesc));
		dbDesc.ByteWidth = sizeof(LightColourBuffer) * Scene::kMaxLights;
		dbDesc.Usage = D3D11_USAGE_DYNAMIC;
		dbDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		//dbDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
		dbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		dbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		dbDesc.StructureByteStride = sizeof(LightColourBuffer);
		if (FAILED(m_pDevice->CreateBuffer(&dbDesc, NULL, &m_pLightColourBuffer)))
		{
			return false;
		}

		dbDesc.ByteWidth = sizeof(LightPosBuffer) * Scene::kMaxLights;
		dbDesc.StructureByteStride = sizeof(LightPosBuffer);
		if (FAILED(m_pDevice->CreateBuffer(&dbDesc, NULL, &m_pLightPosBuffer)))
		{
			return false;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
		ZeroMemory(&descSRV, sizeof(descSRV));
		descSRV.Format = DXGI_FORMAT_UNKNOWN;
		descSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		descSRV.Buffer.FirstElement = 0;
		descSRV.Buffer.NumElements = Scene::kMaxLights;
		descSRV.Buffer.ElementWidth = Scene::kMaxLights;

		if (FAILED(m_pDevice->CreateShaderResourceView(m_pLightColourBuffer, &descSRV, &m_pLightColourView)))
		{
			return false;
		}

		if (FAILED(m_pDevice->CreateShaderResourceView(m_pLightPosBuffer, &descSRV, &m_pLightPosView)))
		{
			return false;
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

		g_GlobalMatrices.view = m_pSceneManager->GetActiveCamera()->GetViewMatrix();
		SetPerspectiveMatrix(m_pSceneManager->GetActiveCamera());
		MapBufferData(m_pGlobalMatrixBuffer, &g_GlobalMatrices, sizeof(GlobalMatrixBuffer));
		SetConstantBuffer(m_pGlobalMatrixBuffer, 0, ShaderType::Vertex);

		int numOfLights = 0;
		for (auto light : m_pSceneManager->m_LightList)
		{
			g_LightPosData[numOfLights].brightness = light->GetBrightness();
			g_LightPosData[numOfLights].position = light->WorldMatrix().Position();
			g_LightColourData[numOfLights].color = light->GetColour();
			g_LightColourData[numOfLights].alpha = 0.0f;
			++numOfLights;
		}

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
				SetConstantBuffer(m_pObjMatrixBuffer, 1, &g_ObjMatrix, sizeof(ObjectMatrixBuffer), ShaderType::Vertex);
				m_pDeviceContext->DrawIndexed((*itr).first->GetIndexCount(), 0, 0);
			}
		}*/


		///////////////////////////
		// Lighting compute


		///////////////////////////
		// Lighting pass

		m_pModelShader->SetShader(m_pDeviceContext);

		g_GlobalLightData.ambientColour = {0.1f, 0.1f, 0.1f, 1.0f};
		g_GlobalLightData.cameraPos = gen::CVector4(m_pSceneManager->GetActiveCamera()->WorldMatrix().Position());
		g_GlobalLightData.specularPower = 32;
		g_GlobalLightData.numOfLights = numOfLights;

		MapBufferData(m_pGlobalLightDataBuffer, &g_GlobalLightData, sizeof(GlobalLightDataBuffer));
		MapBufferData(m_pLightPosBuffer, &g_LightPosData, sizeof(LightPosBuffer) * Scene::kMaxLights);
		MapBufferData(m_pLightColourBuffer, &g_LightColourData, sizeof(LightColourBuffer) * Scene::kMaxLights);

		SetConstantBuffer(m_pGlobalLightDataBuffer, 0, ShaderType::Pixel);
		SetStructuredBuffer(m_pLightPosView, 2, ShaderType::Pixel);
		SetStructuredBuffer(m_pLightColourView, 3, ShaderType::Pixel);

		for (auto itr = m_pSceneManager->m_ModelMap.begin(); itr != m_pSceneManager->m_ModelMap.end(); ++itr)
		{
			(*itr).first->SetBuffers(m_pDeviceContext);
			auto& modelList = (*itr).second;

			for (auto modelItr = modelList.begin(); modelItr != modelList.end(); ++modelItr)
			{
				g_ObjMatrix.world = (*modelItr)->WorldMatrix();
				MapBufferData(m_pObjMatrixBuffer, &g_ObjMatrix, sizeof(ObjectMatrixBuffer));
				SetConstantBuffer(m_pObjMatrixBuffer, 1, ShaderType::Vertex);

				Material* pMat = (*modelItr)->GetMaterial();
				g_MaterialData.diffuseColour = pMat->GetDiffuseColour();
				g_MaterialData.alpha = pMat->GetAlpha();
				g_MaterialData.dirtyness = pMat->GetDirtyness();
				g_MaterialData.shinyness = pMat->GetShinyness();
				g_MaterialData.hasAlpha = pMat->HasAlpha() ? 1 : 0;
				g_MaterialData.hasDirt = pMat->HasDirt() ? 1 : 0;
				g_MaterialData.hasDiffuseTex = pMat->HasDiffuseTex() ? 1 : 0;
				g_MaterialData.hasSpecularTex = pMat->HasSpecularTex() ? 1 : 0;

				if (pMat->HasDiffuseTex())
				{
					m_pDeviceContext->PSSetShaderResources(0, 1, pMat->GetDiffuseTexPtr());
				}

				MapBufferData(m_pMaterialBuffer, &g_MaterialData, sizeof(MaterialBuffer));
				SetConstantBuffer(m_pMaterialBuffer, 1, ShaderType::Pixel);

				m_pDeviceContext->DrawIndexed((*itr).first->GetIndexCount(), 0, 0);
			}
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
	}

	//Copys the data to a buffer
	void DXRenderDevice::MapBufferData(ID3D11Buffer* buffer, void* data, unsigned int dataSize)
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
	}

	//Sets a constant buffer to a shader
	void DXRenderDevice::SetConstantBuffer(ID3D11Buffer* buffer, unsigned int bufferIndex, ShaderType type)
	{
		switch (type)
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

	//Sets a structured buffer to a shader
	void DXRenderDevice::SetStructuredBuffer(ID3D11ShaderResourceView* resource, unsigned int resourceIndex, ShaderType type)
	{
		switch (type)
		{
		case ShaderType::Vertex:
			m_pDeviceContext->VSSetShaderResources(resourceIndex, 1, &resource);
			break;
		case ShaderType::Pixel:
			m_pDeviceContext->PSSetShaderResources(resourceIndex, 1, &resource);
			break;
		case ShaderType::Compute:
			m_pDeviceContext->CSSetShaderResources(resourceIndex, 1, &resource);
			break;
		}
	}

	//Creates and sets the perspective matrix from a camera
	void DXRenderDevice::SetPerspectiveMatrix(Scene::Camera* camera)
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
		g_GlobalMatrices.projection.Set(mat4x4.m[0]);

		/*g_GlobalMatrices.projection.e00 = yScale;
		g_GlobalMatrices.projection.e11 = xScale;
		g_GlobalMatrices.projection.e22 = camera->GetFarClip() / (camera->GetFarClip() - camera->GetNearClip());
		g_GlobalMatrices.projection.e32 = (-camera->GetFarClip() - (-(camera->GetNearClip()))) / (camera->GetFarClip() - camera->GetNearClip());
		g_GlobalMatrices.projection.e23 = 1.0f;
		g_GlobalMatrices.projection.e33 = 0.0f;

		g_GlobalMatrices.projection.Transpose();*/
	}
}