#pragma once
#include "DXGraphics\DXIncludes.h"
#include "DXGraphics\DXCommon.h"

namespace DXG
{
	template <typename StructType>
	class StructuredBuffer
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Creates a buffer 
		StructuredBuffer()
		{
		}

		//Destructor ensures clean up of DirectX objects and cpu data
		~StructuredBuffer()
		{
			SAFE_RELEASE(m_pResourceView);
			SAFE_RELEASE(m_pUnorderedAccessView);
			SAFE_RELEASE(m_pDataBuffer);

			if (m_pData != nullptr)
			{
				delete[] m_pData;
				m_pData = nullptr;
			}
		}

		//Initialises the buffer with a set size
		bool Init(ID3D11Device* pDevice, uint size, CPUAccess access = CPUAccess::None, bool isUAV = false, bool isCounter = false)
		{
			//static_assert(sizeof(StructType) % 16 == 0, "Struct not divisable by 16 bytes");

			m_CPUAccess = access;
			m_IsUAV = isUAV;

			//Grab the memory used to copy data to and from the buffer
			if (m_CPUAccess != CPUAccess::None)
			{
				m_pData = new StructType[size];
			}
			m_DataSize = sizeof(StructType) * size;

			//Create the buffer
			D3D11_BUFFER_DESC bufferDesc;
			ZeroMemory(&bufferDesc, sizeof(bufferDesc));
			bufferDesc.ByteWidth = m_DataSize;

			if ((m_CPUAccess & CPUAccess::Read) != 0)
			{
				bufferDesc.Usage = D3D11_USAGE_STAGING;
				bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
			}
			else if((m_CPUAccess & CPUAccess::Write) != 0)
			{
				bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
				bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
				bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			}
			else
			{
				bufferDesc.Usage = D3D11_USAGE_DEFAULT;
				bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				if (m_IsUAV) bufferDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
			}
			bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
			bufferDesc.StructureByteStride = sizeof(StructType);
			if (FAILED(pDevice->CreateBuffer(&bufferDesc, NULL, &m_pDataBuffer)))
			{
				return false;
			}

			if (bufferDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
			{
				//Create the resource view
				D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
				ZeroMemory(&descSRV, sizeof(descSRV));
				descSRV.Format = DXGI_FORMAT_UNKNOWN;
				descSRV.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
				descSRV.Buffer.FirstElement = 0;
				descSRV.Buffer.NumElements = size;
				descSRV.Buffer.ElementWidth = sizeof(StructType);
				if (FAILED(pDevice->CreateShaderResourceView(m_pDataBuffer, &descSRV, &m_pResourceView)))
				{
					return false;
				}
			}

			if (bufferDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
			{
				D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
				ZeroMemory(&uavDesc, sizeof(uavDesc));
				uavDesc.Format = DXGI_FORMAT_UNKNOWN;
				uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
				uavDesc.Buffer.FirstElement = 0;
				uavDesc.Buffer.NumElements = size;
				uavDesc.Buffer.Flags = isCounter ? D3D11_BUFFER_UAV_FLAG_COUNTER : 0;

				if (FAILED(pDevice->CreateUnorderedAccessView(m_pDataBuffer, &uavDesc, &m_pUnorderedAccessView)))
				{
					return false;
				}

			}

			return true;
		}

		///////////////////////////
		// Data access

		//Copies the struct data at the index in the array
		inline void Set(const uint index, const StructType& data) { m_pData[index] = data; m_IsDirty = true; }

		//Read only access
		inline const StructType& Get(const uint index) { return m_pData[index]; }

		//Enables direct access but does not set dirty flag
		inline StructType& operator[] (const uint index) { return m_pData[index]; }

		//Sets the state to dirty so that the data is sent to the graphics card next commit
		inline void SetDirty() { m_IsDirty = true; }

		inline ID3D11ShaderResourceView* GetShaderView() { return m_pResourceView; }
		inline ID3D11UnorderedAccessView* GetUnorderedAccessView() { return m_pUnorderedAccessView; }

		///////////////////////////
		// Data update/transfer

		//Sends the data to the graphics card if and only if changes have been made since the previous update
		void CommitChanges(ID3D11DeviceContext* pDeviceContext)
		{
			if (m_IsDirty && CPUAccess::Write)
			{
				DXG::MapBufferData(pDeviceContext, m_pDataBuffer, m_pData, m_DataSize);

				m_IsDirty = false;
			}
		}

		//Sets the buffer to be accessible to the specific shader
		void Bind(ID3D11DeviceContext* pDeviceContext, ShaderType sType, uint index, BufferType type = BufferType::Structured, uint* initialCounts = nullptr)
		{
			CommitChanges(pDeviceContext);

			if (type == BufferType::Structured && m_pResourceView != nullptr)
			{
				switch (sType)
				{
				case ShaderType::Vertex:
					pDeviceContext->VSSetShaderResources(index, 1, &m_pResourceView);
					break;
				case ShaderType::Hull:
					pDeviceContext->HSSetShaderResources(index, 1, &m_pResourceView);
					break;
				case ShaderType::Domain:
					pDeviceContext->DSSetShaderResources(index, 1, &m_pResourceView);
					break;
				case ShaderType::Geometry:
					pDeviceContext->GSSetShaderResources(index, 1, &m_pResourceView);
					break;
				case ShaderType::Pixel:
					pDeviceContext->PSSetShaderResources(index, 1, &m_pResourceView);
					break;
				case ShaderType::Compute:
					pDeviceContext->CSSetShaderResources(index, 1, &m_pResourceView);
					break;
				}
			}
			else if (type == BufferType::UAV && m_pUnorderedAccessView != nullptr)
			{
				switch (sType)
				{
				case ShaderType::Compute:
					pDeviceContext->CSSetUnorderedAccessViews(index, 1, &m_pUnorderedAccessView, initialCounts);
					break;
				}
			}
		}

		void Unbind(ID3D11DeviceContext* pDeviceContext, ShaderType sType, uint index, BufferType type = BufferType::Structured)
		{
			if (type == BufferType::Structured && m_pResourceView != nullptr)
			{
				ID3D11ShaderResourceView* clearView[] = {nullptr};
				switch (sType)
				{
				case ShaderType::Vertex:
					pDeviceContext->VSSetShaderResources(index, 1, clearView);
					break;
				case ShaderType::Hull:
					pDeviceContext->HSSetShaderResources(index, 1, clearView);
					break;
				case ShaderType::Domain:
					pDeviceContext->DSSetShaderResources(index, 1, clearView);
					break;
				case ShaderType::Geometry:
					pDeviceContext->GSSetShaderResources(index, 1, clearView);
					break;
				case ShaderType::Pixel:
					pDeviceContext->PSSetShaderResources(index, 1, clearView);
					break;
				case ShaderType::Compute:
					pDeviceContext->CSSetShaderResources(index, 1, clearView);
					break;
				}
			}
			else if (type == BufferType::UAV && m_pUnorderedAccessView != nullptr)
			{
				ID3D11UnorderedAccessView* clearView[] = { nullptr };
				switch (sType)
				{
				case ShaderType::Compute:
					pDeviceContext->CSSetUnorderedAccessViews(index, 1, clearView, nullptr);
					break;
				}
			}
		}

		void Clear(ID3D11DeviceContext* pDeviceContext)
		{
			if (m_pUnorderedAccessView != NULL)
			{
				float clearColor[4] = { 0, 0, 0, 0 };
				pDeviceContext->ClearUnorderedAccessViewFloat(m_pUnorderedAccessView, clearColor);
			}
		}

	private:
		CPUAccess m_CPUAccess = CPUAccess::None;
		StructType* m_pData = nullptr;
		uint m_DataSize;
		bool m_IsDirty = false;
		bool m_IsUAV;

		ID3D11Buffer* m_pDataBuffer = NULL;
		ID3D11ShaderResourceView* m_pResourceView = NULL;
		ID3D11UnorderedAccessView* m_pUnorderedAccessView = NULL;
	};
}