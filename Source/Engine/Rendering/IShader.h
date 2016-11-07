#pragma once
#include "DXIncludes.h"

//Inspired by www.rastertek.com/dx11s2tut04.html

namespace Render
{
	class IShader
	{
	public:
		//Initialises the shader and returns whether it was successful
		virtual bool Init(ID3D11Device* pDevice) = 0;

		//Cleans up everything used by the shader
		virtual void Shutdown() = 0;

		virtual bool Render() = 0;
	};
}