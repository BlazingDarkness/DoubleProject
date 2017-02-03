#pragma once
#include "DXGraphics\DXIncludes.h"
#include "DXGraphics\DXCommon.h"

namespace DXG
{
	template <typename StructType>
	class ConstantBuffer
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Creates a buffer 
		ConstantBuffer()
		{
			m_pDataBuffer = NULL;
			ZeroMemory(&m_Data, sizeof(StructType));
			m_DataSize = sizeof(StructType);
		}

		//Destructor ensures clean up of DirectX objects and cpu data
		~ConstantBuffer()
		{
			SAFE_RELEASE(m_pDataBuffer);
		}

		//Initialises the buffer with a set size
		bool Init(ID3D11Device* pDevice)
		{
			static_assert(sizeof(StructType) % 16 == 0, "Struct not divisable by 16 bytes");

			//Create the buffer
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.ByteWidth = m_DataSize;
			bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufferDesc.MiscFlags = 0;
			if (FAILED(pDevice->CreateBuffer(&bufferDesc, NULL, &m_pDataBuffer)))
			{
				return false;
			}

			return true;
		}

		///////////////////////////
		// Data access

		//Copies the struct data
		inline void Set(const StructType& data) { m_Data = data; m_IsDirty = true; }

		//Read only access
		inline const StructType& Get() { return m_Data; }

		//Mutable access, sets dirty flag to true
		inline StructType& GetMutable() {  m_IsDirty = true; return m_Data; }

		//Sets the state to dirty so that the data is sent to the graphics card next commit
		inline void SetDirty() { m_IsDirty = true; }

		///////////////////////////
		// Data update/transfer

		//Sends the data to the graphics card if and only if changes have been made since the previous update
		void CommitChanges(ID3D11DeviceContext* pDeviceContext)
		{
			if (m_IsDirty)
			{
				DXG::MapBufferData(pDeviceContext, m_pDataBuffer, &m_Data, m_DataSize);

				m_IsDirty = false;
			}
		}

		//Sets the buffer to be accessible to the specific shader
		void Bind(ID3D11DeviceContext* pDeviceContext, ShaderType sType, uint index)
		{
			CommitChanges(pDeviceContext);

			switch (sType)
			{
			case ShaderType::Vertex:
				pDeviceContext->VSSetConstantBuffers(index, 1, &m_pDataBuffer);
				return;
			case ShaderType::Hull:
				pDeviceContext->HSSetConstantBuffers(index, 1, &m_pDataBuffer);
				return;
			case ShaderType::Domain:
				pDeviceContext->DSSetConstantBuffers(index, 1, &m_pDataBuffer);
				return;
			case ShaderType::Geometry:
				pDeviceContext->GSSetConstantBuffers(index, 1, &m_pDataBuffer);
				return;
			case ShaderType::Pixel:
				pDeviceContext->PSSetConstantBuffers(index, 1, &m_pDataBuffer);
				return;
			case ShaderType::Compute:
				pDeviceContext->CSSetConstantBuffers(index, 1, &m_pDataBuffer);
				return;
			}
		}

		void Unbind(ID3D11DeviceContext* pDeviceContext, ShaderType sType, uint index)
		{
			ID3D11Buffer* clearBuffer[] = {nullptr};
			switch (sType)
			{
			case ShaderType::Vertex:
				pDeviceContext->VSSetConstantBuffers(index, 1, clearBuffer);
				return;
			case ShaderType::Hull:
				pDeviceContext->HSSetConstantBuffers(index, 1, clearBuffer);
				return;
			case ShaderType::Domain:
				pDeviceContext->DSSetConstantBuffers(index, 1, clearBuffer);
				return;
			case ShaderType::Geometry:
				pDeviceContext->GSSetConstantBuffers(index, 1, clearBuffer);
				return;
			case ShaderType::Pixel:
				pDeviceContext->PSSetConstantBuffers(index, 1, clearBuffer);
				return;
			case ShaderType::Compute:
				pDeviceContext->CSSetConstantBuffers(index, 1, clearBuffer);
				return;
			}
		}

	private:
		StructType m_Data;
		uint m_DataSize;
		bool m_IsDirty = false;

		ID3D11Buffer* m_pDataBuffer;
	};
}