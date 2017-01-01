#pragma once
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>

#define SAFE_RELEASE(x) if(x != NULL) x->Release(); x = NULL;
#define SAFE_DELETE(x) if(x != NULL) delete x; x = NULL;

#define AL16 __declspec(align(16))
#define AL4 __declspec(align(4))

namespace DXG
{
	using uint = unsigned int;

	//Shader types are listed numerically in their respective order within the graphics pipeline
	//Note: The compute shader is not a part of the graphics piepline
	enum ShaderType
	{
		Vertex,
		Hull,
		Domain,
		Geometry,
		Pixel,
		Compute
	};
}