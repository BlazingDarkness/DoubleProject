#pragma once
//#include "IRenderDevice.h"
#include "DXGraphics\DXIncludes.h"
#include "DXGraphics\ConstantBuffer.h"
#include "DXGraphics\StructuredBuffer.h"
#include "Rendering\MeshManager.h"
#include "Rendering\TextureManager.h"
#include "Rendering\MaterialManager.h"
#include "Shaders\ModelShader.h"
#include "Shaders\DepthShader.h"
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


		///////////////////////////
		// Variables

		ID3D11Device*			m_pDevice = NULL;
		ID3D11DeviceContext*	m_pDeviceContext = NULL;
		IDXGISwapChain*			m_pSwapChain = NULL;
		ID3D11Texture2D*        m_pDepthStencilBuffer = NULL;
		ID3D11DepthStencilState* m_pDepthStencilState = NULL;
		ID3D11DepthStencilView* m_pDepthStencilView = NULL;
		ID3D11RasterizerState*	m_pRasterState = NULL;
		ID3D11RenderTargetView* m_pRenderTargetView = NULL;
		ID3D11SamplerState*		m_pSamplerState = NULL;

		//Constant Buffers
		template<typename T>
		using ConstBuffer = DXG::ConstantBuffer<T>;

		ConstBuffer<GlobalMatrix>* m_GlobalMatrixConstBuffer;
		ConstBuffer<ObjectMatrix>* m_ObjMatrixConstBuffer;
		ConstBuffer<GlobalLightData>* m_GlobalLightConstBuffer;
		ConstBuffer<MaterialData>* m_MaterialConstBuffer;

		//Structured Buffers
		DXG::StructuredBuffer<Light>* m_pLightStructuredBuffer;

		unsigned int m_ScreenWidth;
		unsigned int m_ScreenHeight;

		Scene::Manager* m_pSceneManager = nullptr;
		MeshManager* m_pMeshManager = nullptr;
		TextureManager* m_pTextureManager = nullptr;
		MaterialManager* m_pMaterialManager = nullptr;


		ModelShader* m_pModelShader = nullptr;
		DepthShader* m_pDepthShader = nullptr;
	};
}