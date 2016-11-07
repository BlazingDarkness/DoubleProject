#pragma once
#include "Rendering\DXIncludes.h"
#include <string>

namespace Render
{
	class Mesh
	{
		///////////////////////////
		// Construct / destruction

		//Initialises stuff to null
		Mesh();

		//Cleans up all stuff used by DirectX
		~Mesh();

		//Attempts to load a mesh file
		//Returns false if failed
		bool Load(ID3D11Device* pDevice, const std::string& fileName);

	private:
		ID3D11Buffer* m_pVertexBuffer;
		ID3D11InputLayout* m_pLayout; //position, normal, UVs etc
		ID3D11Buffer* m_pIndexBuffer;

		unsigned int m_IndexCount;
		unsigned int m_VertexCount;
		unsigned int m_VertexSize;
	};
}