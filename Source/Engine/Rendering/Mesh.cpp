#include "Rendering\Mesh.h"
#include "CImportXFile.h"

namespace Render
{
	///////////////////////////
	// Construct / destruction

	//Initialises stuff to null
	Mesh::Mesh()
	{
		m_pIndexBuffer = NULL;
		m_pVertexBuffer = NULL;
		m_pLayout = NULL;

		m_IndexCount = 0;
		m_VertexCount = 0;
		m_VertexSize = 0;
	}

	//Cleans up all stuff used by DirectX
	Mesh::~Mesh()
	{
		SAFE_RELEASE(m_pIndexBuffer);
		SAFE_RELEASE(m_pVertexBuffer);
		SAFE_RELEASE(m_pLayout);
	}

	//Attempts to load a mesh file
	//Returns false if failed
	bool Mesh::Load(ID3D11Device* pDevice, const std::string& fileName)
	{
		if (pDevice == NULL) return false;

		// Use CImportXFile class (from another application) to load the given file. The import code is wrapped in the namespace 'gen'
		gen::CImportXFile mesh;
		if (mesh.ImportFile(fileName.c_str()) != gen::kSuccess)
		{
			return false;
		}

		// Get first sub-mesh from loaded file
		gen::SSubMesh subMesh;
		if (mesh.GetSubMesh(0, &subMesh, false) != gen::kSuccess)
		{
			return false;
		}

		//To do

	}

}