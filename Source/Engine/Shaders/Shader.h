#pragma once
#include "Rendering\DXIncludes.h"
#include <string>
#include <vector>

namespace Render
{
	enum ShaderType
	{
		Pixel,
		Vertex,
		Compute
	};

	class Shader
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Sets all defaults
		Shader();

		//Ensures cleanup of any DX stuff
		~Shader();

		//Initialises the shader and returns whether it was successful
		bool Init(ID3D11Device* pDevice, const ShaderType type, const std::string& shaderFile);


		///////////////////////////
		// Gets & Sets

		//Sets the shaders to the device context
		void SetShader(ID3D11DeviceContext* pContext);

		//Returns the type of shaders
		ShaderType GetType() { return m_Type; }

	private:
		union
		{
			ID3D11VertexShader* m_pVertexShader;
			ID3D11PixelShader* m_pPixelShader;
			ID3D11ComputeShader* m_pComputeShader;
		};
		ID3D11InputLayout* m_pLayout;
		ShaderType m_Type;
	};
}