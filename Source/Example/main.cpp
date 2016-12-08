#include "Engine.h"
#include "Input.h"

const int kLightRows = 4;
const int kLightCols = 4;
const int kNumOfColours = 4;
const gen::CVector3 kLightColours[kNumOfColours] = { Scene::Light::kBlue, Scene::Light::kWhite, Scene::Light::kGreen, Scene::Light::kRed };


Scene::Model* g_pTeapotModel = nullptr;
Scene::Model* g_pFloorModel = nullptr;
Scene::Camera* g_pCamera = nullptr;
Scene::Light* g_pLights[kLightRows * kLightCols] = { nullptr };
Render::Material* g_pMoonMaterial = nullptr;
Render::Material* g_pWoodMaterial = nullptr;

//Returns true if all items in scene were successfully created
bool CreateScene()
{
	//Models
	g_pTeapotModel = Engine::SceneManager()->CreateModel("..\\..\\Media\\Teapot.x");
	if (g_pTeapotModel == nullptr) return false;
	g_pFloorModel = Engine::SceneManager()->CreateModel("..\\..\\Media\\Floor.x");
	if (g_pFloorModel == nullptr) return false;

	//Materials
	g_pMoonMaterial = Engine::MaterialManager()->CreateMaterial("Moon", "..\\..\\Media\\Moon.jpg", 1.0f);
	g_pWoodMaterial = Engine::MaterialManager()->CreateMaterial("Wood", "..\\..\\Media\\wood2.jpg", 0.1f);

	g_pFloorModel->SetMaterial(g_pWoodMaterial);

	//Lights
	for (int row = 0; row < kLightRows; ++row)
	{
		for (int col = 0; col < kLightCols; ++col)
		{
			g_pLights[row * kLightCols + 0] = Engine::SceneManager()->CreateLight(kLightColours[col % kNumOfColours], 5.0f);
			g_pLights[row * kLightCols + 0]->Matrix().SetPosition({ 75.0f - (50.0f * static_cast<float>(col)), 10.0f, 75.0f - (50.0f * static_cast<float>(row)) });
		}
	}

	//Camera
	g_pCamera = Engine::SceneManager()->CreateCamera();
	if (g_pCamera == nullptr) return false;

	Engine::SceneManager()->SetActiveCamera(g_pCamera);

	g_pTeapotModel->Matrix().SetPosition({0.0f, 0.0f, 0.0f});
	g_pFloorModel->Matrix().SetPosition({ 0.0f, 0.0f, 0.0f });
	g_pCamera->Matrix().SetPosition({ 0.0f, 10.0f, -50.0f });
	
	return true;
}

void Controls(float delta)
{
	const float kCameraSpeed = 50.0f;
	//Camera rotation
	if (KeyHeld(EKeyCode::Key_Up))
	{
		g_pCamera->Matrix().RotateLocalX(-1.0f * delta);
	}
	else if (KeyHeld(EKeyCode::Key_Down))
	{
		g_pCamera->Matrix().RotateLocalX(1.0f * delta);
	}
	else if (KeyHeld(EKeyCode::Key_Left))
	{
		g_pCamera->Matrix().RotateLocalY(-1.0f * delta);
	}
	else if (KeyHeld(EKeyCode::Key_Right))
	{
		g_pCamera->Matrix().RotateLocalY(1.0f * delta);
	}

	//Camera movement
	if (KeyHeld(EKeyCode::Key_W))
	{
		g_pCamera->Matrix().MoveLocalZ(kCameraSpeed * delta);
	}
	else if (KeyHeld(EKeyCode::Key_S))
	{
		g_pCamera->Matrix().MoveLocalZ(-kCameraSpeed * delta);
	}
	else if (KeyHeld(EKeyCode::Key_D))
	{
		g_pCamera->Matrix().MoveLocalX(kCameraSpeed * delta);
	}
	else if (KeyHeld(EKeyCode::Key_A))
	{
		g_pCamera->Matrix().MoveLocalX(-kCameraSpeed * delta);
	}
	else if (KeyHeld(EKeyCode::Key_Q))
	{
		g_pCamera->Matrix().MoveLocalY(kCameraSpeed * delta);
	}
	else if (KeyHeld(EKeyCode::Key_E))
	{
		g_pCamera->Matrix().MoveLocalY(-kCameraSpeed * delta);
	}

	//Model movement
	if (KeyHeld(EKeyCode::Key_T))
	{
		g_pTeapotModel->Matrix().MoveLocalZ(20.0f * delta);
	}
	else if (KeyHeld(EKeyCode::Key_G))
	{
		g_pTeapotModel->Matrix().MoveLocalZ(-20.0f * delta);
	}
	else if (KeyHeld(EKeyCode::Key_H))
	{
		g_pTeapotModel->Matrix().MoveLocalX(20.0f * delta);
	}
	else if (KeyHeld(EKeyCode::Key_F))
	{
		g_pTeapotModel->Matrix().MoveLocalX(-20.0f * delta);
	}

	//Model rotation
	if (KeyHeld(EKeyCode::Key_I))
	{
		g_pTeapotModel->Matrix().RotateLocalX(-1.0f * delta);
	}
	else if (KeyHeld(EKeyCode::Key_K))
	{
		g_pTeapotModel->Matrix().RotateLocalX(1.0f * delta);
	}
	else if (KeyHeld(EKeyCode::Key_J))
	{
		g_pTeapotModel->Matrix().RotateLocalY(-1.0f * delta);
	}
	else if (KeyHeld(EKeyCode::Key_L))
	{
		g_pTeapotModel->Matrix().RotateLocalY(1.0f * delta);
	}

	//Material switcher
	if (KeyHeld(EKeyCode::Key_0))
	{
		g_pTeapotModel->SetMaterial(&Render::g_DefaultMaterial);
		Render::g_DefaultMaterial = Render::Material("default", { 0.7f, 0.7f, 0.7f, 1.0f });
	}
	else if (KeyHeld(EKeyCode::Key_1))
	{
		g_pTeapotModel->SetMaterial(&Render::g_DefaultMaterial);
		Render::g_DefaultMaterial = Render::Material("default", { 1.0f, 0.5f, 0.0f, 1.0f });
	}
	else if (KeyHeld(EKeyCode::Key_2))
	{
		g_pTeapotModel->SetMaterial(&Render::g_DefaultMaterial);
		Render::g_DefaultMaterial = Render::Material("default", { 0.0f, 0.8f, 0.8f, 1.0f });
	}
	else if (KeyHeld(EKeyCode::Key_3))
	{
		g_pTeapotModel->SetMaterial(g_pMoonMaterial);
	}
	else if (KeyHeld(EKeyCode::Key_4))
	{
		g_pTeapotModel->SetMaterial(g_pWoodMaterial);
	}
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
			Controls(delta);

			//At the end of the scene update
			Engine::Render();
			delta = Engine::Update();
		}
	}

	//Ensure engine is shut down
	Engine::ShutDown();

	return (int)(Engine::LastMessage().wParam);
}