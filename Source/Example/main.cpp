#include "Engine.h"
#include "Input.h"

Scene::Model* g_pTeapotModel = nullptr;
Scene::Camera* g_pCamera = nullptr;

//Returns true if all items in scene were successfully created
bool CreateScene()
{
	/*Render::Mesh* pTeapotMesh =  Engine::MeshManager()->LoadMesh("..\\..\\Media\\Teapot.x");
	if (pTeapotMesh == nullptr) return false;*/
	g_pTeapotModel = Engine::SceneManager()->CreateModel("..\\..\\Media\\Teapot.x");
	if (g_pTeapotModel == nullptr) return false;
	g_pCamera = Engine::SceneManager()->CreateCamera();
	if (g_pCamera == nullptr) return false;
	Engine::SceneManager()->SetActiveCamera(g_pCamera);

	g_pTeapotModel->Matrix().SetPosition({0.0f, 0.0f, 0.0f});
	g_pCamera->Matrix().SetPosition({ 0.0f, 5.0f, -10.0f });


	
	return true;
}


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	// Initialise everything
	if (!(Engine::Init(hInstance, nCmdShow)))
	{
		return (int)(Engine::LastMessage().wParam);
	}

	if (Engine::IsRunning() && CreateScene())
	{
		float delta = 0.0f;
		Engine::Render();
		delta = Engine::Update();
	
		while (Engine::IsRunning())
		{
			//Update stuff here

			//Camera rotation
			if (KeyHeld(EKeyCode::Key_Up))
			{
				g_pCamera->Matrix().RotateLocalX(1.0f * delta);
			}
			else if (KeyHeld(EKeyCode::Key_Down))
			{
				g_pCamera->Matrix().RotateLocalX(-1.0f * delta);
			}
			else if (KeyHeld(EKeyCode::Key_Left))
			{
				g_pCamera->Matrix().RotateLocalY(1.0f * delta);
			}
			else if (KeyHeld(EKeyCode::Key_Right))
			{
				g_pCamera->Matrix().RotateLocalY(-1.0f * delta);
			}
			//Camera movement
			if (KeyHeld(EKeyCode::Key_W))
			{
				g_pCamera->Matrix().MoveLocalZ(10.0f * delta);
			}
			else if (KeyHeld(EKeyCode::Key_S))
			{
				g_pCamera->Matrix().MoveLocalZ(-10.0f * delta);
			}
			else if (KeyHeld(EKeyCode::Key_D))
			{
				g_pCamera->Matrix().MoveLocalX(10.0f * delta);
			}
			else if (KeyHeld(EKeyCode::Key_A))
			{
				g_pCamera->Matrix().MoveLocalX(-10.0f * delta);
			}
			//Model movement
			if (KeyHeld(EKeyCode::Key_T))
			{
				g_pTeapotModel->Matrix().MoveLocalZ(1.0f * delta);
			}
			else if (KeyHeld(EKeyCode::Key_G))
			{
				g_pTeapotModel->Matrix().MoveLocalZ(-1.0f * delta);
			}
			else if (KeyHeld(EKeyCode::Key_H))
			{
				g_pTeapotModel->Matrix().MoveLocalX(1.0f * delta);
			}
			else if (KeyHeld(EKeyCode::Key_F))
			{
				g_pTeapotModel->Matrix().MoveLocalX(-1.0f * delta);
			}

			//At the end of the scene update
			Engine::Render();
			delta = Engine::Update();
		}
	}

	//Ensure engine is shut down
	Engine::ShutDown();

	return (int)(Engine::LastMessage().wParam);
}