#pragma once
#include "DXGraphics\DXIncludes.h"
#include <string>

namespace Render
{
	class Mesh
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Initialises stuff to null
		Mesh();

		//Cleans up all stuff used by DirectX
		~Mesh();

		//Attempts to load a mesh file
		//Returns false if failed
		bool Load(ID3D11Device* pDevice, const std::string& fileName);

		//Sets the mesh's buffers to the directX device
		void SetBuffers(ID3D11DeviceContext* pDeviceContext);

		//Returns the file name from which the mesh was loaded
		std::string GetFileName() { return m_FileName; }

		//Returns the number of vertices in the mesh
		unsigned int GetVertexCount() { return m_VertexCount; }

		//Returns the number of indices in the mesh
		unsigned int GetIndexCount() { return m_IndexCount; }

	private:
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11Buffer* m_pIndexBuffer;

		unsigned int m_IndexCount;
		unsigned int m_VertexCount;
		unsigned int m_VertexSize;

		std::string m_FileName;
	};
}