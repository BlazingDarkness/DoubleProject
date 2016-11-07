#pragma once
#include "DepthShader.h"

namespace Render
{
	///////////////////////////
	// Construct / destruction

	//Sets all defaults
	DepthShader::DepthShader()
	{

	}

	//Ensures cleanup of any DX stuff
	DepthShader::~DepthShader()
	{
		m_pVertexShader = NULL;
		m_pPixelShader = NULL;
		m_pLayout = NULL;
	}

	//Initialises the shader and returns whether it was successful
	bool DepthShader::Init(ID3D11Device* pDevice)
	{

	}

	//Cleans up everything used by the shader
	void DepthShader::Shutdown()
	{

	}

	bool DepthShader::Render()
	{

	}
}