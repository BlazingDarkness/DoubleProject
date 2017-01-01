#pragma once
#include "DXGraphics\DXIncludes.h"

namespace DXG
{
	//Copys the data to a buffer
	void MapBufferData(ID3D11DeviceContext* pDeviceContext, ID3D11Buffer* buffer, void* data, unsigned int dataSize);
}