#pragma once
#include "DXGraphics\DXIncludes.h"
#include "DXGraphics\DXCommon.h"
#include "DXGraphics\IDXResource.h"

namespace DXG
{
	//template <typename StructType>
	class Texture2D : public IDXResource
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Creates a texture
		Texture2D()
		{
		}

		//Destructor ensures clean up of DirectX objects and cpu data
		~Texture2D()
		{
			SAFE_RELEASE(m_pResourceView);
			SAFE_RELEASE(m_pUnorderedAccessView);
			SAFE_RELEASE(m_pTex2D);
		}

		//Initialises the buffer with a set size
		bool Init(ID3D11Device* pDevice, uint width, uint height)
		{
			//static_assert(sizeof(StructType) % 16 == 0, "Struct not divisable by 16 bytes");

			m_IsUAV = true;

			//Create the buffer
			D3D11_TEXTURE2D_DESC texDesc;
			ZeroMemory(&texDesc, sizeof(texDesc));
			texDesc.ArraySize = 1;
			texDesc.Width = width;
			texDesc.Height = height;
			texDesc.Usage = D3D11_USAGE_DEFAULT;
			texDesc.Format = DXGI_FORMAT_R32G32_UINT;
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
			texDesc.SampleDesc.Count = 1;
			texDesc.SampleDesc.Quality = 0;
			texDesc.MipLevels = 1;

			if (FAILED(pDevice->CreateTexture2D(&texDesc, NULL, &m_pTex2D)))
			{
				return false;
			}

			if (texDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
			{
				//Create the resource view
				D3D11_SHADER_RESOURCE_VIEW_DESC descSRV;
				ZeroMemory(&descSRV, sizeof(descSRV));
				descSRV.Format = DXGI_FORMAT_R32G32_UINT;
				descSRV.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
				descSRV.Texture2D.MipLevels = 1;
				descSRV.Texture2D.MostDetailedMip = 0;
				if (FAILED(pDevice->CreateShaderResourceView(m_pTex2D, &descSRV, &m_pResourceView)))
				{
					return false;
				}
			}

			if (texDesc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
			{
				D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
				ZeroMemory(&uavDesc, sizeof(uavDesc));
				uavDesc.Format = DXGI_FORMAT_UNKNOWN;
				uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
				uavDesc.Texture2D.MipSlice = 0;

				if (FAILED(pDevice->CreateUnorderedAccessView(m_pTex2D, &uavDesc, &m_pUnorderedAccessView)))
				{
					return false;
				}
			}

			return true;
		}

		//Sets the texture to be accessible to the specific shader
		virtual void Bind(ID3D11DeviceContext* pDeviceContext, ShaderType sType, uint index, BufferType bufferType)
		{
			if (bufferType == BufferType::Structured && m_pResourceView != nullptr)
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
			else if (bufferType == BufferType::UAV && m_pUnorderedAccessView != nullptr)
			{
				switch (sType)
				{
				case ShaderType::Compute:
					pDeviceContext->CSSetUnorderedAccessViews(index, 1, &m_pUnorderedAccessView, nullptr);
					break;
				}
			}
		}

		virtual void Unbind(ID3D11DeviceContext* pDeviceContext, ShaderType sType, uint index, BufferType bufferType)
		{
			if (bufferType == BufferType::Structured && m_pResourceView != nullptr)
			{
				ID3D11ShaderResourceView* clearView[] = { nullptr };
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
			else if (bufferType == BufferType::UAV && m_pUnorderedAccessView != nullptr)
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

	private:
		uint m_DataSize;
		bool m_IsUAV;

		ID3D11Texture2D* m_pTex2D = NULL;
		ID3D11ShaderResourceView* m_pResourceView = NULL;
		ID3D11UnorderedAccessView* m_pUnorderedAccessView = NULL;
	};
}