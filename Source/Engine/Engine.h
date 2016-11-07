#pragma once
#include <windows.h>
#include <windowsx.h>
#include "DXRenderDevice.h"
#include "Scene\Manager.h"

class Engine
{
public:
	///////////////////////////
	// Construct / destruction

	//Creates a basic engine
	Engine();

	//Destroys engine
	~Engine();

	///////////////////////////
	// Intialisation

	//Initialises the engine
	//Returns false if failed
	bool Init(HINSTANCE hInstance, int nCmdShow);

	//Starts the engine running
	MSG Start();

private:
	//Creates window
	//Returns false if failed
	bool InitWindow(HINSTANCE hInstance, int nCmdShow);


	///////////////////////////
	// Variables

	HINSTANCE m_hInst = NULL;
	HWND      m_hWnd = NULL;
	Render::DXRenderDevice* m_pRenderDevice;
	Scene::Manager* m_pSceneManager;
};