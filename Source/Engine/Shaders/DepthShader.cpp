#include "Shaders\DepthShader.h"
#include <fstream>

namespace Render
{
	///////////////////////////
	// Construct / destruction

	//Sets all defaults
	DepthShader::DepthShader()
	{
		m_pVertexShader = nullptr;
		m_pPixelShader = nullptr;
	}

	//Ensures cleanup of any DX stuff
	DepthShader::~DepthShader()
	{
		SAFE_DELETE(m_pVertexShader);
		SAFE_DELETE(m_pPixelShader);
	}

	//Initialises the shader and returns whether it was successful
	//Returns true if successful
	bool DepthShader::Init(ID3D11Device* pDevice, const std::string& vertexShader, const std::string& pixelShader)
	{
		m_pPixelShader = new DXG::Shader();
		if (!m_pPixelShader->Init(pDevice, DXG::ShaderType::Pixel, pixelShader)) return false;

		m_pVertexShader = new DXG::Shader();
		if (!m_pVertexShader->Init(pDevice, DXG::ShaderType::Vertex, vertexShader)) return false;

		return true;
	}


	///////////////////////////
	// Sets

	//Sets the shaders to the device context
	void DepthShader::SetTechnique(ID3D11DeviceContext* pContext)
	{
		m_pVertexShader->SetShader(pContext);
		m_pPixelShader->SetShader(pContext);
	}

	//Runs the shader
	void DepthShader::Run()
	{

	}
}