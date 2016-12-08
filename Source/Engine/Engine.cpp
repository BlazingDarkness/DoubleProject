#include "Engine.h"
#include "Input.h"
#include "Resource.h"
#include <windows.h>
#include <windowsx.h>


//Functions
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

///////////////////////////
// Static variables
Engine* Engine::m_pEngine = nullptr;
MSG Engine::msg = {0};


///////////////////////////
// Intialisation

//Initialises the engine
//Returns false if failed
bool Engine::Init(HINSTANCE hInstance, int nCmdShow)
{
	//Destroy existing engine
	if (m_pEngine != nullptr)
	{
		m_pEngine->ShutDown();
	}

	m_pEngine = new Engine();

	if (!m_pEngine->InitEngine(hInstance, nCmdShow))
	{
		m_pEngine->ShutDown();
		return false;
	}

	return true;
}

//Manages input etc since last frame
//Returns time delta since last render call
float Engine::Update()
{
	//If no engine exists
	if (m_pEngine == nullptr) return 0.0f;

	float delta = m_pEngine->m_Timer.GetLapTime();
	
	// Main message loop
	// First check to see if there are any messages that need to be processed for the window (window resizing, minimizing, whatever)
	// If not then the window is idle and the D3D rendering occurs. This is in a loop. So the window is rendered over and over, as fast as
	// possible as long as we are not manipulating the window in some way
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	//Shutdown if requested to do so my windows
	if (WM_QUIT == msg.message)
	{
		ShutDown();
	}
	else if (KeyHit(Key_Escape)) // Allow user to quit with escape key
	{
		ShutDown();
	}

	return delta;
}

//Renders the next frame
void Engine::Render()
{
	//If no engine exists
	if (m_pEngine == nullptr) return;

	m_pEngine->m_pRenderDevice->RenderScene();
}

//Deletes all contents and data used by the engine
void Engine::ShutDown()
{
	if (m_pEngine == nullptr) return;

	delete m_pEngine;
	m_pEngine = nullptr;
}


///////////////////////////
// Gets

//Returns a pointer to the mesh manager used in the engine
Render::MeshManager* Engine::MeshManager()
{
	if (m_pEngine == nullptr) return nullptr;

	return m_pEngine->m_pMeshManager;
}

//Returns a pointer to the material manager used in the engine
Render::MaterialManager* Engine::MaterialManager()
{
	if (m_pEngine == nullptr) return nullptr;

	return m_pEngine->m_pMaterialManager;
}

//Returns a pointer to the scene manager used in the engine
Scene::Manager* Engine::SceneManager()
{
	if (m_pEngine == nullptr) return nullptr;

	return m_pEngine->m_pSceneManager;
}

//Returns false if there is no engine currently running
bool Engine::IsRunning()
{
	return (m_pEngine != nullptr && m_pEngine->m_Initialised);
}

//Returns the most recent message acted upon by the engine
MSG Engine::LastMessage()
{
	return msg;
}


///////////////////////////
// Construct / destruction

//Creates a basic engine
Engine::Engine()
{

}

//Destroys engine
Engine::~Engine()
{
	if (m_pRenderDevice) delete m_pRenderDevice;

	m_pRenderDevice = nullptr;
	m_pMeshManager = nullptr;
	m_pSceneManager = nullptr;

	if (m_hWnd != NULL)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}


//Initialises the engine
//Returns false if failed
bool Engine::InitEngine(HINSTANCE hInstance, int nCmdShow)
{
	//Create Window
	if (!InitWindow(hInstance, nCmdShow))
	{
		return false;
	}

	//Create render device
	m_pRenderDevice = new Render::DXRenderDevice();
	if (!m_pRenderDevice->Init(m_hWnd))
	{
		SAFE_DELETE(m_pRenderDevice);
		return false;
	}

	m_pMeshManager = m_pRenderDevice->GetMeshManager();
	m_pMaterialManager = m_pRenderDevice->GetMaterialManager();
	m_pSceneManager = m_pRenderDevice->GetSceneManager();

	// Initialise simple input functions (in Input.cpp) - not DirectX
	InitInput();

	m_Timer.Start();

	m_Initialised = true;

	return true;
}


//Creates window
//Returns false if failed
bool Engine::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = L"ForwardPlusDemo";
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
	if (!RegisterClassEx(&wcex))
		return false;

	// Create window
	m_hInst = hInstance;
	RECT rc = { 0, 0, 1280, 960 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	m_hWnd = CreateWindow(L"ForwardPlusDemo", L"Forward Plus Rendering Demo", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, hInstance,
		NULL);
	if (!m_hWnd)
		return false;

	ShowWindow(m_hWnd, nCmdShow);

	return true;
}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK Engine::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	RECT rect;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		// These windows messages (WM_KEYXXXX) can be used to get keyboard input to the window
		// This application has added some simple functions (not DirectX) to process these messages (all in Input.cpp/h)
	case WM_KEYDOWN:
		KeyDownEvent(static_cast<EKeyCode>(wParam));
		break;

	case WM_KEYUP:
		KeyUpEvent(static_cast<EKeyCode>(wParam));
		break;

		// Get mouse buttons too, treat them as keys
	case WM_LBUTTONDOWN:
		KeyDownEvent(Mouse_LButton);
		break;
	case WM_LBUTTONUP:
		KeyUpEvent(Mouse_LButton);
		break;

	case WM_MBUTTONDOWN:
		KeyDownEvent(Mouse_MButton);
		break;
	case WM_MBUTTONUP:
		KeyUpEvent(Mouse_MButton);
		break;

	case WM_RBUTTONDOWN:
		KeyDownEvent(Mouse_RButton);
		break;
	case WM_RBUTTONUP:
		KeyUpEvent(Mouse_RButton);
		break;

	case WM_MOUSEMOVE:
		//g_MouseX = GET_X_LPARAM(lParam);
		//g_MouseY = GET_Y_LPARAM(lParam);
		break;

	case WM_SIZING:
		//rect = *(reinterpret_cast<RECT*>(lParam));
		if (m_pEngine != nullptr)
		{
			if (SUCCEEDED(GetWindowRect(hWnd, &rect)))
			{
				m_pEngine->m_pRenderDevice->SetScreenHeight(rect.bottom - rect.top);
				m_pEngine->m_pRenderDevice->SetScreenWidth(rect.right - rect.left);
			}
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
