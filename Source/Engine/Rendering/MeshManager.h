#pragma once
#include "Rendering\Mesh.h"
#include <unordered_map>

namespace Render
{
	class MeshManager
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Requires a device for creating meshes
		MeshManager(ID3D11Device* pDevice);

		//Destroys all meshes
		~MeshManager();


		///////////////////////////
		// Creation & Removal

		//Attempts to load a mesh from a file
		//Returns a nullptr if failed
		Mesh* LoadMesh(const std::string& path);

		//Removes the mesh
		void RemoveMesh(Mesh* mesh);

	private:
		///////////////////////////
		// member variables

		using MeshPair = std::pair<std::string, Mesh*>;
		using MeshMap = std::unordered_map<std::string, Mesh*>;

		MeshMap m_MeshMap;
		ID3D11Device* m_pDevice;
	};
}