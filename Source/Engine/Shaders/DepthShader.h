#pragma once
#include "DXGraphics\DXIncludes.h"
#include "DXGraphics\Shader.h"
#include <string>

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
		void SetTechnique(ID3D11DeviceContext* pContext);

		//Runs the shader
		void Run();

	private:
		DXG::Shader* m_pVertexShader;
		DXG::Shader* m_pPixelShader;
	};
}