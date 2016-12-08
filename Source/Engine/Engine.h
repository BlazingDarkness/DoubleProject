#pragma once
#include <windows.h>
#include <windowsx.h>
#include "Rendering\DXRenderDevice.h"
#include "Scene\Manager.h"
#include "CTimer.h"

//singleton engine class
class Engine
{
public:

	///////////////////////////
	// Intialisation

	//Initialises the engine
	//Returns false if failed
	static bool Init(HINSTANCE hInstance, int nCmdShow);

	//Manages input etc since last frame
	//Returns time delta since last render call
	static float Update();

	//Renders the next frame
	static void Render();

	//Deletes all contents and data used by the engine
	static void ShutDown();


	///////////////////////////
	// Gets

	//Returns a pointer to the mesh manager used in the engine
	static Render::MeshManager* MeshManager();

	//Returns a pointer to the material manager used in the engine
	static Render::MaterialManager* MaterialManager();

	//Returns a pointer to the scene manager used in the engine
	static Scene::Manager* SceneManager();

	//Returns false if there is no engine currently running
	static bool IsRunning();

	//Returns the most recent message acted upon by the engine
	static MSG LastMessage();

private:
	///////////////////////////
	// Construct / destruction

	//Creates a basic engine
	Engine();

	//Destroys engine
	~Engine();

	//Initialises the engine
	//Returns false if failed
	bool InitEngine(HINSTANCE hInstance, int nCmdShow);

	//Creates window
	//Returns false if failed
	bool InitWindow(HINSTANCE hInstance, int nCmdShow);

	//Callback
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	///////////////////////////
	// Variables

	static Engine* m_pEngine;
	static MSG msg;

	bool m_Initialised = false;
	HINSTANCE m_hInst = NULL;
	HWND m_hWnd = NULL;
	CTimer m_Timer;
	Render::DXRenderDevice* m_pRenderDevice = nullptr;
	Render::MeshManager* m_pMeshManager = nullptr;
	Render::MaterialManager* m_pMaterialManager = nullptr;
	Scene::Manager* m_pSceneManager = nullptr;
};