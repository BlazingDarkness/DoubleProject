#pragma once
//#include "IRenderDevice.h"
#include "DXGraphics\DXIncludes.h"
#include "DXGraphics\ConstantBuffer.h"
#include "DXGraphics\StructuredBuffer.h"
#include "DXGraphics\Texture2D.h"
#include "DXGraphics\RenderPass.h"
#include "Rendering\MeshManager.h"
#include "Rendering\TextureManager.h"
#include "Rendering\MaterialManager.h"
#include "Shaders\CommonStructs.h"
#include "Scene\Manager.h"

namespace Render
{
	class DXRenderDevice
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Creates a device object but does not initialise
		DXRenderDevice();

		//Releases all memory used
		~DXRenderDevice();

		//Initialises the device
		bool Init(HWND hWnd);


		///////////////////////////
		// Rendering

		//Renders the scene
		void RenderScene();


		///////////////////////////
		// Gets & Sets

		Scene::Manager* GetSceneManager() { return m_pSceneManager; }

		MeshManager* GetMeshManager() { return m_pMeshManager; }

		MaterialManager* GetMaterialManager() { return m_pMaterialManager; }

		void SetScreenWidth(unsigned int screenWidth) { m_ScreenWidth = screenWidth; }

		unsigned int GetScreenWidth() { return m_ScreenWidth; }

		void SetScreenHeight(unsigned int screenHeight) { m_ScreenHeight = screenHeight; }

		unsigned int GetScreenHeight() { return m_ScreenHeight; }


	private:
		///////////////////////////
		// Render steps

		//Resets the back buffer and depth buffers
		void ClearScreen();

		//Creates and sets the perspective matrix from a camera
		gen::CMatrix4x4 CalcPerspectiveMatrix(Scene::Camera* camera);

		//Forward rendering
		void RenderForward();

		//Forward+ rendering
		void RenderForwardPlus();

		//Heatmap rendering
		void RenderHeatmap();

		//Resizes all components dependant on screen size
		bool Resize();


		///////////////////////////
		// Variables
		enum RenderMode {ForwardPlus, Forward, Heatmap};
		RenderMode m_RenderMode = RenderMode::ForwardPlus;

		//Descs - only those needed for screen resizing
		DXGI_SWAP_CHAIN_DESC m_SwapChainDesc;
		D3D11_TEXTURE2D_DESC m_DepthStencilDesc;
		D3D11_TEXTURE2D_DESC m_DepthTextureDesc;

		//DX resources
		ID3D11Device*				m_pDevice = NULL;
		ID3D11DeviceContext*		m_pDeviceContext = NULL;
		IDXGISwapChain*				m_pSwapChain = NULL;
		ID3D11Texture2D*			m_pDepthStencilBuffer = NULL;
		ID3D11DepthStencilState*	m_pDepthStencilState = NULL;
		ID3D11DepthStencilView*		m_pDepthStencilView = NULL;
		ID3D11Texture2D*			m_pDepthTexture = NULL;
		ID3D11ShaderResourceView*	m_pDepthResourceView = NULL;
		ID3D11RenderTargetView*		m_pDepthRenderTargetView = NULL;
		ID3D11RasterizerState*		m_pRasterState = NULL;
		ID3D11RenderTargetView*		m_pRenderTargetView = NULL;
		ID3D11SamplerState*			m_pSamplerState = NULL;

		//Constant Buffers
		template<typename T>
		using ConstBuffer = DXG::ConstantBuffer<T>;

		ConstBuffer<GlobalMatrix>*		m_GlobalMatrixConstBuffer;
		ConstBuffer<ObjectMatrix>*		m_ObjMatrixConstBuffer;
		ConstBuffer<GlobalLightData>*	m_GlobalLightConstBuffer;
		ConstBuffer<MaterialData>*		m_MaterialConstBuffer;
		ConstBuffer<GlobalThreadData>*	m_GlobalThreadConstBuffer;
		ConstBuffer<CopyDetails>*		m_BufferCopyConstBuffer;
		ConstBuffer<FrustumData>*		m_FrustumConstBuffer;

		//Structured Buffers
		template<typename T>
		using StructuredBuffer = DXG::StructuredBuffer<T>;

		StructuredBuffer<Light>*	 m_pLightStructuredBuffer;
		StructuredBuffer<Frustum>*	 m_pFrustumStructuredBuffer;
		StructuredBuffer<DXG::uint>* m_pLightIndexStructuredBuffer;
		StructuredBuffer<DXG::uint>* m_pLightOffsetStructuredBuffer;
		StructuredBuffer<DXG::uint>* m_pZeroedStructuredBuffer;

		//Texture 2Ds
		using Texture2D = DXG::Texture2D;

		Texture2D* m_pLightGrid;

		unsigned int m_PrevScreenWidth;
		unsigned int m_PrevScreenHeight;
		unsigned int m_ScreenWidth;
		unsigned int m_ScreenHeight;
		unsigned int m_TileRows;
		unsigned int m_TileCols;

		Scene::Manager* m_pSceneManager = nullptr;
		MeshManager* m_pMeshManager = nullptr;
		TextureManager* m_pTextureManager = nullptr;
		MaterialManager* m_pMaterialManager = nullptr;

		//Shaders
		DXG::Shader* m_pDepthVS = nullptr;
		DXG::Shader* m_pDepthPS = nullptr;
		DXG::Shader* m_pModelVS = nullptr;
		DXG::Shader* m_pModelPS = nullptr;
		DXG::Shader* m_pLightCullCS = nullptr;
		DXG::Shader* m_pCopyCS  = nullptr;
		DXG::Shader* m_pFrustumCalcCS = nullptr;
		DXG::Shader* m_pHeatMapVS = nullptr;
		DXG::Shader* m_pHeatMapPS = nullptr;
		DXG::Shader* m_pForwardPS = nullptr;
		
		//Render Passes
		DXG::RenderPass m_CopyPass;
		DXG::RenderPass m_LightCullPass;
		DXG::RenderPass m_FullRenderPass;
		DXG::RenderPass m_FrustumPass;
		DXG::RenderPass m_DepthPass;
		DXG::RenderPass m_HeatMapPass;
		DXG::RenderPass m_ForwardPass;

		//Tweakbar vars

	};
}