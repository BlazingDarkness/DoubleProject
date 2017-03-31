#pragma once
#include "DXGraphics\DXIncludes.h"
#include "DXGraphics\DXCommon.h"

namespace DXG
{
	class IDXResource
	{
	public:
		virtual void Bind(ID3D11DeviceContext* pDeviceContext, ShaderType sType, uint index, BufferType type) = 0;
		virtual void Unbind(ID3D11DeviceContext* pDeviceContext, ShaderType sType, uint index, BufferType type) = 0;
	};
}