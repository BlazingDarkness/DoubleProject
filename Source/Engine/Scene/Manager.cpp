#pragma once
#include "Manager.h"

namespace Scene
{
	///////////////////////////
	// Construct / destruction

	//Sets up initial scene
	Manager::Manager(Render::MeshManager* meshManager)
	{
		m_pMeshManager = meshManager;

		m_pActiveCamera = nullptr;

		m_DefaultCamera = Camera();
	}

	//Destroys all scene objects
	Manager::~Manager()
	{
		//Ensure destruction of all lights
		for (auto light = m_LightList.begin(); light != m_LightList.end(); ++light)
		{
			delete (*light);
		}
		m_LightList.clear();

		//Ensure destruction of all cameras
		for (auto camera = m_CameraList.begin(); camera != m_CameraList.end(); ++camera)
		{
			delete (*camera);
		}
		m_CameraList.clear();

		//Ensure destruction of all models
		for (auto modelMapItr = m_ModelMap.begin(); modelMapItr != m_ModelMap.end(); ++modelMapItr)
		{
			for (auto model = modelMapItr->second.begin(); model != modelMapItr->second.end(); ++model)
			{
				delete (*model);
			}
		}
		m_ModelMap.clear();
	}

	///////////////////////////
	// Scene creation

	//Creates a light with a colour and brightness with positional data from a matrix
	Light* Manager::CreateLight(const gen::CVector3& colour, const float brightness, const gen::CMatrix4x4& mat)
	{
		Light* light = new Light(colour, brightness, mat);

		m_LightList.push_back(light);

		return light;
	}

	//Removes the light from the scene
	void Manager::RemoveLight(Light* l)
	{
		for (auto light = m_LightList.begin(); light != m_LightList.end(); ++light)
		{
			if (l == *light)
			{
				m_LightList.erase(light);
				delete l;
				return;
			}
		}
	}

	//Creates a camera with a FOV, near clip, far clip, with positional data from a matrix
	Camera* Manager::CreateCamera(const float FOV, const float nearClip, const float farClip, const gen::CMatrix4x4& mat)
	{
		Camera* camera = new Camera(FOV, nearClip, farClip, mat);

		m_CameraList.push_back(camera);

		return camera;
	}

	//Removes the camera from the scene
	void Manager::RemoveCamera(Camera* c)
	{
		for (auto camera = m_CameraList.begin(); camera != m_CameraList.end(); ++camera)
		{
			if (c == *camera)
			{
				if (c == m_pActiveCamera) m_pActiveCamera = nullptr;
				m_CameraList.erase(camera);
				delete c;
				return;
			}
		}
	}

	//Creates a model from a mesh with positional data from a matrix
	Model* Manager::CreateModel(Render::Mesh* pMesh, const gen::CMatrix4x4& mat)
	{
		//Find or create model list
		auto& modelList = m_ModelMap[pMesh];

		Model* model = new Model(pMesh, mat);
		modelList.push_back(model);

		return model;
	}

	//Creates a model from a mesh file with positional data from a matrix
	//nullptr is returned if unable to read the mesh from the file
	Model* Manager::CreateModel(const std::string& fileName, const gen::CMatrix4x4& mat)
	{
		Render::Mesh* pMesh = m_pMeshManager->LoadMesh(fileName);

		//Check if failed to load mesh
		if (pMesh == nullptr) return nullptr;

		//Find or create model list
		auto& modelList = m_ModelMap[pMesh];

		Model* model = new Model(pMesh, mat);
		modelList.push_back(model);

		return model;
	}

	//Removes the model from the scene
	void Manager::RemoveModel(Model* m)
	{
		ModelMap::iterator mapItr = m_ModelMap.find(m->GetMesh());

		if (mapItr == m_ModelMap.end())
		{
			return;
		}

		for (auto model = mapItr->second.begin(); model != mapItr->second.end(); ++model)
		{
			if (m == *model)
			{
				mapItr->second.erase(model);
				delete m;
				return;
			}
		}
	}


	///////////////////////////
	// Gets & Sets

	//Sets the camera that the renderer will use to render the scene
	void Manager::SetActiveCamera(Camera* c)
	{
		m_pActiveCamera = c;
	}

	//Returns the camera that the renderer will use to render the scene
	//If there is no active camera then a nullptr is returned
	Camera* Manager::GetActiveCamera()
	{
		if (m_pActiveCamera == nullptr)
		{
			return &m_DefaultCamera;
		}

		return m_pActiveCamera;
	}
}