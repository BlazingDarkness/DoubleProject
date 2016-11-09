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

		m_IndexCount = 0;
		m_VertexCount = 0;
		m_VertexSize = 0;
	}

	//Cleans up all stuff used by DirectX
	Mesh::~Mesh()
	{
		SAFE_RELEASE(m_pIndexBuffer);
		SAFE_RELEASE(m_pVertexBuffer);
	}

	//Attempts to load a mesh file
	//Returns false if failed
	bool Mesh::Load(ID3D11Device* pDevice, const std::string& fileName)
	{
		if (pDevice == NULL) return false;
		m_FileName = fileName;

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

		//calculate vertex size
		m_VertexSize = 12;
		if (subMesh.hasNormals)
		{
			m_VertexSize += 12;
		}
		if (subMesh.hasTangents)
		{
			m_VertexSize += 12;
		}
		if (subMesh.hasTextureCoords)
		{
			m_VertexSize += 8;
		}
		if (subMesh.hasVertexColours)
		{
			m_VertexSize += 4;
		}

		// Create the vertex buffer and fill it with the loaded vertex data
		m_VertexCount = subMesh.numVertices;
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT; // Not a dynamic buffer
		bufferDesc.ByteWidth = m_VertexCount * m_VertexSize; // Buffer size
		bufferDesc.CPUAccessFlags = 0;   // Indicates that CPU won't access this buffer at all after creation
		bufferDesc.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA initData; // Initial data
		initData.pSysMem = subMesh.vertices;
		
		if (FAILED(pDevice->CreateBuffer(&bufferDesc, &initData, &m_pVertexBuffer)))
		{
			return false;
		}

		// Create the index buffer - assuming 2-byte (WORD) index data
		m_IndexCount = static_cast<unsigned int>(subMesh.numFaces) * 3;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.ByteWidth = m_IndexCount * sizeof(WORD);
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		initData.pSysMem = subMesh.faces;
		if (FAILED(pDevice->CreateBuffer(&bufferDesc, &initData, &m_pIndexBuffer)))
		{
			return false;
		}
	}

	//Sets the mesh's buffers to the directX device
	void Mesh::SetBuffers(ID3D11DeviceContext* pDeviceContext)
	{
		unsigned int offset;
		pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &m_VertexSize, &offset);
		pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	}
}