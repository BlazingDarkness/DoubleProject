#include "Rendering\MeshManager.h"

namespace Render
{
	///////////////////////////
	// Construct / destruction

	//Requires a device for creating meshes
	MeshManager::MeshManager(ID3D11Device* pDevice)
	{
		m_pDevice = pDevice;
	}

	//Destroys all meshes
	MeshManager::~MeshManager()
	{
		for (auto mesh = m_MeshMap.begin(); mesh != m_MeshMap.end(); ++mesh)
		{
			delete (*mesh).second;
		}
		m_MeshMap.clear();
	}


	///////////////////////////
	// Creation & Removal

	//Attempts to load a mesh from a file
	//Returns a nullptr if failed
	Mesh* MeshManager::LoadMesh(const std::string& path)
	{
		//Check to see if it already exists
		auto itr = m_MeshMap.find(path);
		if (itr != m_MeshMap.end())
		{
			return itr->second;
		}

		//Create mesh
		Mesh* mesh = new Mesh;
		if (mesh->Load(m_pDevice, path))
		{
			m_MeshMap.insert(MeshPair{path, mesh});

			return mesh;
		}

		return nullptr;
	}

	//Removes the mesh
	void MeshManager::RemoveMesh(Mesh* mesh)
	{
		m_MeshMap.erase(mesh->GetFileName());
		delete mesh;
	}
}