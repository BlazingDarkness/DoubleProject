#pragma once
//#include "IRenderDevice.h"
#include "Rendering\DXIncludes.h"
#include "Rendering\MeshManager.h"
#include "Shaders\ModelShader.h"
#include "Shaders\DepthShader.h"
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

		unsigned int GetScreenWidth() { return m_ScreenWidth; }

		unsigned int GetScreenHeight() { return m_ScreenHeight; }


	private:
		///////////////////////////
		// Render steps

		//Resets the back buffer and depth buffers
		void ClearScreen();

		//Copys the data to a constant buffer
		void SetConstantBuffer(ID3D11Buffer* buffer, unsigned int bufferIndex, void* data, unsigned int dataSize, ShaderType type);


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

		//Constant Buffers
		ID3D11Buffer* m_pMatrixBuffer = NULL;

		unsigned int m_ScreenWidth;
		unsigned int m_ScreenHeight;

		Scene::Manager* m_pSceneManager = nullptr;
		MeshManager* m_pMeshManager = nullptr;
		ModelShader* m_pModelShader = nullptr;
		DepthShader* m_pDepthShader = nullptr;

		struct MatrixBuffer
		{
			gen::CMatrix4x4 world;
			gen::CMatrix4x4 view;
			gen::CMatrix4x4 projection;
		} m_Matrices;
	};
}