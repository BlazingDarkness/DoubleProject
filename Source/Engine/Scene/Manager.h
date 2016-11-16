#pragma once
#include "Scene/Light.h"
#include "Scene/Model.h"
#include "Scene/Camera.h"
#include "Rendering/MeshManager.h"
#include "Rendering/TextureManager.h"
#include <list>
#include <map>

namespace Render {
	class DXRenderDevice;
}
namespace Scene
{

	class Manager
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Sets up initial scene
		Manager(Render::MeshManager* meshManager);

		//Destroys all scene objects
		~Manager();


		///////////////////////////
		// Scene creation

		//Creates a light with a colour and brightness with positional data from a matrix
		Light* CreateLight(const gen::CVector3& colour = Light::kWhite, const float brightness = 1.0f, const gen::CMatrix4x4& mat = gen::CMatrix4x4::kIdentity);

		//Removes the light from the scene
		void RemoveLight(Light* l);

		//Creates a camera with a FOV, near clip, far clip, with positional data from a matrix
		Camera* CreateCamera(const float FOV = 90.f, const float nearClip = 10.0f, const float farClip = 5000.f, const gen::CMatrix4x4& mat = gen::CMatrix4x4::kIdentity);

		//Removes the camera from the scene
		void RemoveCamera(Camera* c);

		//Creates a model from a mesh with positional data from a matrix
		Model* CreateModel(Render::Mesh* pMesh, const gen::CMatrix4x4& mat = gen::CMatrix4x4::kIdentity);

		//Creates a model from a mesh file with positional data from a matrix
		//nullptr is returned if unable to read the mesh from the file
		Model* CreateModel(const std::string& fileName, const gen::CMatrix4x4& mat = gen::CMatrix4x4::kIdentity);

		//Removes the model from the scene
		void RemoveModel(Model* m);


		///////////////////////////
		// Gets & Sets

		//Sets the camera that the renderer will use to render the scene
		void SetActiveCamera(Camera* c);

		//Returns the camera that the renderer will use to render the scene
		//If there is no active camera then a nullptr is returned
		Camera* GetActiveCamera();


	private:
		///////////////////////////
		// type defs

		using LightList = std::list<Light*>;
		using CameraList = std::list<Camera*>;
		using ModelList = std::list<Model*>;
		using ModelMap = std::map<Render::Mesh*, ModelList>;


		///////////////////////////
		// member variables

		LightList m_LightList;
		CameraList m_CameraList;
		ModelMap m_ModelMap;

		Camera* m_pActiveCamera;
		Camera m_DefaultCamera;

		Render::MeshManager* m_pMeshManager;
		friend Render::DXRenderDevice;
	};
}