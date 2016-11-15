#include "Engine.h"

bool CreateScene()
{
	Render::Mesh* pTeapotMesh =  Engine::MeshManager()->LoadMesh("..\\..\\Media\\Teapot.x");
	if (pTeapotMesh == nullptr) return false;
	Scene::Model* pTeapotModel = Engine::SceneManager()->CreateModel(pTeapotMesh);
	if (pTeapotModel == nullptr) return false;

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


			//At the end of the scene update
			Engine::Render();
			delta = Engine::Update();
		}
	}

	//Ensure engine is shut down
	Engine::ShutDown();

	return (int)(Engine::LastMessage().wParam);
}