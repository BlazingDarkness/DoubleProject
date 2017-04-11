#include "Engine.h"
#include "Input.h"

const int kLightRows = 80;
const int kLightCols = 80;
const int kTeapotRows = 30;
const int kTeapotCols = 30;
const int kNumOfLights = kLightRows * kLightCols;
const int kNumOfTeapots = kTeapotRows * kTeapotCols;
const int kNumOfColours = 4;
const gen::CVector3 kLightColours[kNumOfColours] = { Scene::Light::kBlue, Scene::Light::kWhite, Scene::Light::kGreen, Scene::Light::kRed };

struct LightEntity
{
	Scene::Light* light;
	gen::CVector3 direction;
} g_pLights[kNumOfLights] = { {nullptr, {0.0f, 0.0f, 0.0f}} };

Scene::Model* g_pTeapotModel = nullptr;
Scene::Model* g_pTeapotModelArray[kNumOfTeapots] = { nullptr };
Scene::Model* g_pFloorModel = nullptr;
Scene::Camera* g_pCamera = nullptr;
Render::Material* g_pMoonMaterial = nullptr;
Render::Material* g_pWoodMaterial = nullptr;
Render::Material* g_pOrangeMaterial = nullptr;
Render::Material* g_pCyanMaterial = nullptr;
Render::Material* g_pMatGreyMaterial = nullptr;

//Returns true if all items in scene were successfully created
bool CreateScene()
{
	//Models
	g_pTeapotModel = Engine::SceneManager()->CreateModel("..\\..\\Media\\Teapot.x");
	if (g_pTeapotModel == nullptr) return false;
	g_pFloorModel = Engine::SceneManager()->CreateModel("..\\..\\Media\\Floor.x");
	if (g_pFloorModel == nullptr) return false;

	for (int row = 0; row < kTeapotRows; ++row)
	{
		for (int col = 0; col < kTeapotCols; ++col)
		{
			int index = row * kTeapotCols + col;
			g_pTeapotModelArray[index] = Engine::SceneManager()->CreateModel("..\\..\\Media\\Teapot.x");
			g_pTeapotModelArray[index]->Matrix().SetPosition({ (25.0f * static_cast<float>(col - kTeapotCols / 2)), 0.0f, (25.0f * static_cast<float>(row - kTeapotRows / 2)) });
		}
	}

	//Materials
	g_pMoonMaterial = Engine::MaterialManager()->CreateMaterial("Moon", "..\\..\\Media\\Moon.jpg", 1.0f);
	g_pWoodMaterial = Engine::MaterialManager()->CreateMaterial("Wood", "..\\..\\Media\\wood2.jpg", 0.5f);
	g_pCyanMaterial = Engine::MaterialManager()->CreateMaterial("Cyan", gen::CVector4{ 0.0f, 0.8f, 0.8f, 1.0f }, 1.0f);
	g_pOrangeMaterial = Engine::MaterialManager()->CreateMaterial("Orange", gen::CVector4{ 1.0f, 0.5f, 0.0f, 1.0f }, 1.0f);
	g_pMatGreyMaterial = Engine::MaterialManager()->CreateMaterial("Mat Grey", gen::CVector4{ 0.7f, 0.7f, 0.7f, 1.0f }, 0.0f);

	g_pFloorModel->SetMaterial(g_pWoodMaterial);

	//Lights
	for (int row = 0; row < kLightRows; ++row)
	{
		for (int col = 0; col < kLightCols; ++col)
		{
			int index = row * kLightCols + col;
			g_pLights[index] = { Engine::SceneManager()->CreateLight(kLightColours[col % kNumOfColours], 10.0f, 50.0f), gen::CVector3::kZero };
			g_pLights[index].light->Matrix().SetPosition({ (20.0f * static_cast<float>(col - kLightCols / 2)), 15.0f, (20.0f * static_cast<float>(row - kLightRows / 2)) });
			g_pLights[index].direction = gen::CVector3(gen::Sin(static_cast<float>(index)), 0.0f, gen::Cos(static_cast<float>(index)));
		}
	}

	//Camera
	g_pCamera = Engine::SceneManager()->CreateCamera(90.0f, 1.0f, 10000.0f);
	if (g_pCamera == nullptr) return false;

	Engine::SceneManager()->SetActiveCamera(g_pCamera);

	g_pTeapotModel->Matrix().SetPosition({ 0.0f, 0.0f, 0.0f });
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
	if (KeyHeld(EKeyCode::Key_Down))
	{
		g_pCamera->Matrix().RotateLocalX(1.0f * delta);
	}
	if (KeyHeld(EKeyCode::Key_Left))
	{
		g_pCamera->Matrix().RotateLocalY(-1.0f * delta);
	}
	if (KeyHeld(EKeyCode::Key_Right))
	{
		g_pCamera->Matrix().RotateLocalY(1.0f * delta);
	}

	//Camera movement
	if (KeyHeld(EKeyCode::Key_W))
	{
		g_pCamera->Matrix().MoveLocalZ(kCameraSpeed * delta);
	}
	if (KeyHeld(EKeyCode::Key_S))
	{
		g_pCamera->Matrix().MoveLocalZ(-kCameraSpeed * delta);
	}
	if (KeyHeld(EKeyCode::Key_D))
	{
		g_pCamera->Matrix().MoveLocalX(kCameraSpeed * delta);
	}
	if (KeyHeld(EKeyCode::Key_A))
	{
		g_pCamera->Matrix().MoveLocalX(-kCameraSpeed * delta);
	}
	if (KeyHeld(EKeyCode::Key_Q))
	{
		g_pCamera->Matrix().MoveLocalY(kCameraSpeed * delta);
	}
	if (KeyHeld(EKeyCode::Key_E))
	{
		g_pCamera->Matrix().MoveLocalY(-kCameraSpeed * delta);
	}

	//Model movement
	if (KeyHeld(EKeyCode::Key_T))
	{
		g_pTeapotModel->Matrix().MoveLocalZ(20.0f * delta);
	}
	if (KeyHeld(EKeyCode::Key_G))
	{
		g_pTeapotModel->Matrix().MoveLocalZ(-20.0f * delta);
	}
	if (KeyHeld(EKeyCode::Key_H))
	{
		g_pTeapotModel->Matrix().MoveLocalX(20.0f * delta);
	}
	if (KeyHeld(EKeyCode::Key_F))
	{
		g_pTeapotModel->Matrix().MoveLocalX(-20.0f * delta);
	}

	//Model rotation
	if (KeyHeld(EKeyCode::Key_I))
	{
		g_pTeapotModel->Matrix().RotateLocalX(-1.0f * delta);
	}
	if (KeyHeld(EKeyCode::Key_K))
	{
		g_pTeapotModel->Matrix().RotateLocalX(1.0f * delta);
	}
	if (KeyHeld(EKeyCode::Key_J))
	{
		g_pTeapotModel->Matrix().RotateLocalY(-1.0f * delta);
	}
	if (KeyHeld(EKeyCode::Key_L))
	{
		g_pTeapotModel->Matrix().RotateLocalY(1.0f * delta);
	}

	//Material switcher
	if (KeyHit(EKeyCode::Key_0))
	{
		g_pTeapotModel->SetMaterial(&Render::g_DefaultMaterial);
	}
	if (KeyHit(EKeyCode::Key_1))
	{
		g_pTeapotModel->SetMaterial(g_pOrangeMaterial);
	}
	if (KeyHit(EKeyCode::Key_2))
	{
		g_pTeapotModel->SetMaterial(g_pCyanMaterial);
	}
	if (KeyHit(EKeyCode::Key_3))
	{
		g_pTeapotModel->SetMaterial(g_pMoonMaterial);
	}
	if (KeyHit(EKeyCode::Key_4))
	{
		g_pTeapotModel->SetMaterial(g_pWoodMaterial);
	}
	if (KeyHit(EKeyCode::Key_5))
	{
		g_pTeapotModel->SetMaterial(g_pMatGreyMaterial);
	}
}

void Update(float delta)
{
	for (int i = 0; i < kNumOfLights; ++i)
	{
		g_pLights[i].direction = (gen::MatrixRotationY(gen::kfPi * 0.5f * delta) * gen::CVector4(g_pLights[i].direction, 0.0f)).Vector3();
		g_pLights[i].light->Matrix().Move(delta * 100.0f * g_pLights[i].direction);
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
			Update(delta);

			//At the end of the scene update
			Engine::Render();
			delta = Engine::Update();
		}
	}

	//Ensure engine is shut down
	Engine::ShutDown();

	return (int)(Engine::LastMessage().wParam);
}