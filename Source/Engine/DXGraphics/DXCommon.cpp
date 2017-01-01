#pragma once
#include "DXGraphics\DXCommon.h"

namespace DXG
{
	//Copys the data to a buffer
	void MapBufferData(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* buffer, void* data, unsigned int dataSize)
	{
		//This function's code was taken in part from Rastertek's tutorial dx11s2tut04
		//Then heavily simplified for general use

		HRESULT hr;
		D3D11_MAPPED_SUBRESOURCE resource;

		// Lock the constant buffer so it can be written to.
		hr = pDeviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		if (FAILED(hr))
		{
			return;
		}

		// Copy data to the buffer
		memcpy(resource.pData, data, dataSize);

		// Unlock the constant buffer.
		pDeviceContext->Unmap(buffer, 0);
	}
}