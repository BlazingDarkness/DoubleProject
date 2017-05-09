#include "Engine.h"
#include "Input.h"
#include "AntTweakBar.h"

const int kMaxLightRows = 80;
const int kMaxLightCols = 80;
const int kMaxNumOfLights = kMaxLightRows * kMaxLightCols;

const int kMaxTeapotRows = 30;
const int kMaxTeapotCols = 30;
const int kMaxNumOfTeapots = kMaxTeapotRows * kMaxTeapotCols;

const int kNumOfCityBuildings = 20;
const int kNumOfColours = 4;
const gen::CVector3 kLightColours[kNumOfColours] = { Scene::Light::kBlue, Scene::Light::kWhite, Scene::Light::kGreen, Scene::Light::kRed };

enum SceneMode {Teapot, City};

SceneMode g_SceneMode = SceneMode::Teapot;

struct LightEntity
{
	Scene::Light* light;
	gen::CVector3 direction;
} g_pLights[kMaxNumOfLights] = { {nullptr, {0.0f, 0.0f, 0.0f}} };

struct TeapotMaterials
{
	Render::Material* moon = nullptr;
	Render::Material* wood = nullptr;
	Render::Material* orange = nullptr;
	Render::Material* cyan = nullptr;
	Render::Material* grey = nullptr;
	Render::Material* matGrey = nullptr;
} g_TeapotMaterials;

Render::Material* g_pFloorMaterial = nullptr;
Render::Material* g_pCityMaterials[kNumOfCityBuildings] = {nullptr};

Scene::Model* g_pTeapotModels[kMaxNumOfTeapots] = { nullptr };
Scene::Model* g_pCityModels[kNumOfCityBuildings] = { nullptr };
Scene::Model* g_pFloorModel = nullptr;
Scene::Camera* g_pCamera = nullptr;

int g_LightRows = 0;
int g_LightCols = 0;
int g_NumOfLights = g_LightRows * g_LightCols;

int g_TeapotRows = 30;
int g_TeapotCols = 30;
int g_NumOfTeapots = g_TeapotRows * g_TeapotCols;

float g_LightSpeed = 100.0f;
float g_LightRange = 50.0f;

//Forward declarations
bool CreateScene(SceneMode mode);
void DestroyScene(SceneMode mode);

void SafeRemoveModel(Scene::Model*& model)
{
	if (model != nullptr)
	{
		Engine::SceneManager()->RemoveModel(model);
		model = nullptr;
	}
}

void SafeRemoveMaterial(Render::Material*& material)
{
	if (material != nullptr)
	{
		Engine::MaterialManager()->RemoveMaterial(material);
		material = nullptr;
	}
}

void TW_CALL SetSceneModeCB(const void *value, void * /*clientData*/)
{
	SceneMode mode = *static_cast<const SceneMode*>(value);
	if (mode != g_SceneMode)
	{
		DestroyScene(g_SceneMode);
		g_SceneMode = mode;
		CreateScene(g_SceneMode);
	}
}

void TW_CALL GetSceneModeCB(void *value, void * /*clientData*/)
{
	*static_cast<SceneMode*>(value) = g_SceneMode;
}

void TW_CALL SetLightRangeCB(const void *value, void * /*clientData*/)
{
	g_LightRange = *static_cast<const float*>(value);
	for (int i = 0; i < g_NumOfLights; ++i)
	{
		g_pLights[i].light->SetRange(g_LightRange);
	}
}

void TW_CALL GetLightRangeCB(void *value, void * /*clientData*/)
{
	*static_cast<float*>(value) = g_LightRange;
}

bool SetupTweakBar()
{
	TwBar* bar = TwGetBarByName("Settings");
	if (bar == nullptr) return false;

	TwEnumVal sceneMode[] = { { SceneMode::Teapot, "Teapot" }, { SceneMode::City, "Village" } };
	TwType sceneModeType = TwDefineEnum("SceneModeEnum", sceneMode, 2);
	TwAddVarCB(bar, "SceneMode", sceneModeType, SetSceneModeCB, GetSceneModeCB, NULL, "label='Mode' group='Scene'");
	TwAddVarRW(bar, "Speed", TW_TYPE_FLOAT, &g_LightSpeed, "min=0 max=500 step=5 group='Lights'");
	TwAddVarCB(bar, "Range", TW_TYPE_FLOAT, SetLightRangeCB, GetLightRangeCB, NULL, "min=10 max=150 step=1 group='Lights'");


	return true;
}

bool CreateScene(SceneMode mode)
{
	//Camera
	if (g_pCamera == nullptr) g_pCamera = Engine::SceneManager()->CreateCamera(90.0f, 1.0f, 10000.0f);
	if (g_pCamera == nullptr) return false;
	g_pCamera->Matrix().SetPosition({ 0.0f, 10.0f, -50.0f });
	Engine::SceneManager()->SetActiveCamera(g_pCamera);

	switch (mode)
	{
	case SceneMode::Teapot:
	{
		//Models
		g_pFloorModel = Engine::SceneManager()->CreateModel("..\\..\\Media\\Floor.x");
		if (g_pFloorModel == nullptr) return false;

		for (int row = 0; row < g_TeapotRows; ++row)
		{
			for (int col = 0; col < g_TeapotCols; ++col)
			{
				int index = row * g_TeapotCols + col;
				g_pTeapotModels[index] = Engine::SceneManager()->CreateModel("..\\..\\Media\\Teapot.x");
				g_pTeapotModels[index]->Matrix().SetPosition({ (25.0f * static_cast<float>(col - g_TeapotCols / 2)), 0.0f, (25.0f * static_cast<float>(row - g_TeapotRows / 2)) });
			}
		}

		//Materials
		g_TeapotMaterials.moon = Engine::MaterialManager()->CreateMaterial("Moon", "..\\..\\Media\\Moon.jpg", 1.0f);
		g_TeapotMaterials.wood = Engine::MaterialManager()->CreateMaterial("Wood", "..\\..\\Media\\wood2.jpg", 0.5f);
		g_TeapotMaterials.cyan = Engine::MaterialManager()->CreateMaterial("Cyan", gen::CVector4{ 0.0f, 0.8f, 0.8f, 1.0f }, 1.0f);
		g_TeapotMaterials.orange = Engine::MaterialManager()->CreateMaterial("Orange", gen::CVector4{ 1.0f, 0.5f, 0.0f, 1.0f }, 1.0f);
		g_TeapotMaterials.grey = Engine::MaterialManager()->CreateMaterial("Mat Grey", gen::CVector4{ 0.7f, 0.7f, 0.7f, 1.0f }, 1.0f);
		g_TeapotMaterials.matGrey = Engine::MaterialManager()->CreateMaterial("Mat Grey", gen::CVector4{ 0.7f, 0.7f, 0.7f, 1.0f }, 0.0f);

		g_pFloorModel->SetMaterial(g_TeapotMaterials.wood);

		g_pFloorModel->Matrix().SetPosition({ 0.0f, 0.0f, 0.0f });

	}
	break;
	case SceneMode::City:
	{
		g_pFloorModel = Engine::SceneManager()->CreateModel("..\\..\\Media\\DesertScene\\Ground.x");
		g_pFloorMaterial = Engine::MaterialManager()->CreateMaterial("FloorMat", gen::CVector4(0.5f, 0.5f, 0.5f, 0.0f), 0.5f);

		if (g_pFloorModel == nullptr) return false;
		if (g_pFloorMaterial == nullptr) return false;

		g_pFloorModel->Matrix().Scale(8.0f);
		g_pFloorModel->SetMaterial(g_pFloorMaterial);

		for (int i = 0; i < kNumOfCityBuildings; ++i)
		{
			std::string buildNum = std::to_string(i + 1);
			g_pCityModels[i] = Engine::SceneManager()->CreateModel("..\\..\\Media\\DesertScene\\Building" + buildNum + ".x");
			g_pCityMaterials[i] = Engine::MaterialManager()->CreateMaterial("Building" + buildNum + "Tex", "..\\..\\Media\\DesertScene\\Building" + buildNum + "Tex.png", 0.5f);

			if (g_pCityModels[i] == nullptr) return false;
			if (g_pCityMaterials[i] == nullptr) return false;

			g_pCityModels[i]->SetMaterial(Engine::MaterialManager()->CreateMaterial("Building" + buildNum + "Tex", "..\\..\\Media\\DesertScene\\Building" + buildNum + "Tex.png", 0.5f));
			g_pCityModels[i]->Matrix().Scale(8.0f);
		}
	}
	break;
	}
	return true;
}

void DestroyScene(SceneMode mode)
{
	switch (mode)
	{
	case SceneMode::Teapot:
	{
		SafeRemoveModel(g_pFloorModel);
		SafeRemoveMaterial(g_pFloorMaterial);

		for (int i = 0; i < g_NumOfTeapots; ++i)
		{
			SafeRemoveModel(g_pTeapotModels[i]);
		}

		SafeRemoveMaterial(g_TeapotMaterials.moon);
		SafeRemoveMaterial(g_TeapotMaterials.wood);
		SafeRemoveMaterial(g_TeapotMaterials.cyan);
		SafeRemoveMaterial(g_TeapotMaterials.orange);
		SafeRemoveMaterial(g_TeapotMaterials.grey);
		SafeRemoveMaterial(g_TeapotMaterials.matGrey);
	}
	break;
	case SceneMode::City:
	{
		SafeRemoveModel(g_pFloorModel);
		SafeRemoveMaterial(g_pFloorMaterial);
		
		for (int i = 0; i < kNumOfCityBuildings; ++i)
		{
			SafeRemoveModel(g_pCityModels[i]);
			SafeRemoveMaterial(g_pCityMaterials[i]);
		}
	}
	break;
	}

}

void ChangeLightCount(int row, int col)
{
	int newRowCount = gen::Max(0, gen::Min(row, kMaxLightRows));
	int newColCount = gen::Max(0, gen::Min(col, kMaxLightCols));
	int newLightCount = newRowCount * newColCount;
	if (newLightCount == g_NumOfLights)
	{
		//do nothing
	}
	else if (newLightCount > g_NumOfLights)
	{
		//do nothing
	}
	else if (newLightCount < g_NumOfLights)
	{
		int dif = g_NumOfLights - newLightCount;

		for (int i = 0; i < dif; ++i)
		{
			Engine::SceneManager()->RemoveLight(g_pLights[i].light);
		}

		for (int i = 0; i < newLightCount; ++i)
		{
			g_pLights[i].light = g_pLights[i + dif].light;
		}

		for (int i = newLightCount; i < g_NumOfLights; ++i)
		{
			g_pLights[i].light = nullptr;
		}
	}

	g_LightRows = newRowCount;
	g_LightCols = newColCount;
	g_NumOfLights = newLightCount;

	for (int row = 0; row < g_LightRows; ++row)
	{
		for (int col = 0; col < g_LightCols; ++col)
		{
			int index = row * g_LightCols + col;
			if (g_pLights[index].light == nullptr)
			{
				g_pLights[index] = { Engine::SceneManager()->CreateLight(kLightColours[col % kNumOfColours], 8.0f, 50.0f), gen::CVector3::kZero };
			}
			g_pLights[index].light->Matrix().SetPosition({ (20.0f * static_cast<float>(col - g_LightCols / 2)), 15.0f, (20.0f * static_cast<float>(row - g_LightCols / 2)) });
			g_pLights[index].light->SetColour(kLightColours[col % kNumOfColours]);
			g_pLights[index].direction = gen::CVector3(gen::Sin(static_cast<float>(index)), 0.0f, gen::Cos(static_cast<float>(index)));
		}
	}
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

	if (KeyHit(EKeyCode::Key_X))
	{
		ChangeLightCount(g_LightRows + 1, g_LightCols + 1);
	}
	if (KeyHit(EKeyCode::Key_C))
	{
		ChangeLightCount(g_LightRows - 1, g_LightCols - 1);
	}
	if (KeyHit(EKeyCode::Key_V))
	{
		ChangeLightCount(g_LightRows + 1, g_LightCols);
	}
	if (KeyHit(EKeyCode::Key_B))
	{
		ChangeLightCount(g_LightRows - 1, g_LightCols);
	}
	if (KeyHit(EKeyCode::Key_N))
	{
		ChangeLightCount(g_LightRows, g_LightCols + 1);
	}
	if (KeyHit(EKeyCode::Key_M))
	{
		ChangeLightCount(g_LightRows, g_LightCols - 1);
	}
	if (KeyHeld(EKeyCode::Key_K))
	{
		ChangeLightCount(g_LightRows + 1, g_LightCols + 1);
	}
	if (KeyHeld(EKeyCode::Key_L))
	{
		ChangeLightCount(g_LightRows - 1, g_LightCols - 1);
	}

	//Material switcher
	if (KeyHit(EKeyCode::Key_0))
	{
		if (KeyHeld(EKeyCode::Key_Shift) && g_TeapotMaterials.grey != nullptr)
			Render::g_DefaultMaterial = *g_TeapotMaterials.grey;
	}
	if (KeyHit(EKeyCode::Key_1))
	{
		if (KeyHeld(EKeyCode::Key_Shift) && g_TeapotMaterials.orange != nullptr)
			Render::g_DefaultMaterial = *g_TeapotMaterials.orange;
	}
	if (KeyHit(EKeyCode::Key_2))
	{
		if (KeyHeld(EKeyCode::Key_Shift) && g_TeapotMaterials.cyan != nullptr)
			Render::g_DefaultMaterial = *g_TeapotMaterials.cyan;
	}
	if (KeyHit(EKeyCode::Key_3))
	{
		if (KeyHeld(EKeyCode::Key_Shift) && g_TeapotMaterials.moon != nullptr)
			Render::g_DefaultMaterial = *g_TeapotMaterials.moon;
	}
	if (KeyHit(EKeyCode::Key_4))
	{
		if (KeyHeld(EKeyCode::Key_Shift) && g_TeapotMaterials.wood != nullptr)
			Render::g_DefaultMaterial = *g_TeapotMaterials.wood;
	}
	if (KeyHit(EKeyCode::Key_5))
	{
		if (KeyHeld(EKeyCode::Key_Shift) && g_TeapotMaterials.matGrey != nullptr)
			Render::g_DefaultMaterial = *g_TeapotMaterials.matGrey;
	}
}

void UpdateScene(float delta)
{
	for (int i = 0; i < g_NumOfLights; ++i)
	{
		g_pLights[i].direction = (gen::MatrixRotationY(gen::kfPi * g_LightSpeed * 0.005f * delta) * gen::CVector4(g_pLights[i].direction, 0.0f)).Vector3();
		g_pLights[i].light->Matrix().Move(delta * g_LightSpeed * g_pLights[i].direction);
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	// Initialise everything
	if (!(Engine::Init(hInstance, nCmdShow)))
	{
		return (int)(Engine::LastMessage().wParam);
	}

	if (Engine::IsRunning() && SetupTweakBar() && CreateScene(g_SceneMode))
	{
		float delta = 0.0f;
		Engine::Render();
		delta = Engine::Update();
	
		while (Engine::IsRunning())
		{
			//Update stuff here
			Controls(delta);
			UpdateScene(delta);

			//At the end of the scene update
			Engine::Render();
			delta = Engine::Update();
		}
	}

	//Ensure engine is shut down
	Engine::ShutDown();

	return (int)(Engine::LastMessage().wParam);
}