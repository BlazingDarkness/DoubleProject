#pragma once
#include "Rendering\DXIncludes.h"
#include "CVector4.h"
#include "CMatrix4x4.h"
#include <string>
#include <vector>

namespace Render
{
	class DepthShader
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Sets all defaults
		DepthShader();

		//Ensures cleanup of any DX stuff
		~DepthShader();

		//Initialises the shader and returns whether it was successful
		bool Init(ID3D11Device* pDevice, const std::string& vertexShader, const std::string& pixelShader);


		///////////////////////////
		// Sets

		//Sets the shaders to the device context
		void SetShader(ID3D11DeviceContext* pContext);

		//Runs the shader
		void Run();

	private:
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;
		ID3D11InputLayout* m_pLayout;
	};
}