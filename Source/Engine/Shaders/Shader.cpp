#pragma once
#include "Shaders\Shader.h"
#include <fstream>

namespace Render
{
	///////////////////////////
	// Construct / destruction

	//Sets all defaults
	Shader::Shader()
	{
		m_pLayout = NULL;
		m_pVertexShader = NULL;
	}

	//Ensures cleanup of any DX stuff
	Shader::~Shader()
	{
		SAFE_DELETE(m_pLayout);
		switch (m_Type)
		{
		case ShaderType::Vertex:
			SAFE_DELETE(m_pVertexShader);
			break;
		case ShaderType::Pixel:
			SAFE_DELETE(m_pPixelShader);
			break;
		case ShaderType::Compute:
			SAFE_DELETE(m_pComputeShader);
			break;
		}
	}

	//Initialises the shader and returns whether it was successful
	bool Shader::Init(ID3D11Device* pDevice, const ShaderType type, const std::string& shaderFile)
	{
		m_Type = type;

		std::vector<char> byteCode;

		//Open compiled vertex shader file
		std::ifstream ShaderFile(shaderFile, std::ios::in | std::ios::binary | std::ios::ate);
		if (!ShaderFile.is_open()) return false;

		//Read in shader file
		std::streamoff fileSize = ShaderFile.tellg();
		ShaderFile.seekg(0, std::ios::beg);
		byteCode.resize(static_cast<decltype(byteCode.size())>(fileSize));
		ShaderFile.read(&byteCode[0], fileSize);
		if (ShaderFile.fail()) return false;

		switch (type)
		{
		case ShaderType::Vertex:
			HRESULT hr = pDevice->CreateVertexShader(byteCode.data(), static_cast<SIZE_T>(byteCode.size()), nullptr, &m_pVertexShader);
			if (FAILED(hr)) return false;

			D3D11_INPUT_ELEMENT_DESC inputLayout[] =
			{
				// Semantic     Index  Format                        Slot  Offset  Slot Class                    Instance Step
				{ "POSITION",   0,     DXGI_FORMAT_R32G32B32_FLOAT,  0,    0,      D3D11_INPUT_PER_VERTEX_DATA,  0 },
				{ "NORMAL",     0,     DXGI_FORMAT_R32G32B32_FLOAT,  0,    12,     D3D11_INPUT_PER_VERTEX_DATA,  0 },
				{ "TEXCOORD",   0,     DXGI_FORMAT_R32G32_FLOAT,     0,    24,     D3D11_INPUT_PER_VERTEX_DATA,  0 }
			};
			pDevice->CreateInputLayout(inputLayout, 3, byteCode.data(), static_cast<SIZE_T>(byteCode.size()), &m_pLayout);
			break;

		case ShaderType::Pixel:
			HRESULT hr = pDevice->CreatePixelShader(byteCode.data(), static_cast<SIZE_T>(byteCode.size()), nullptr, &m_pPixelShader);
			if (FAILED(hr)) return false;
			break;

		case ShaderType::Compute:
			HRESULT hr = pDevice->CreateComputeShader(byteCode.data(), static_cast<SIZE_T>(byteCode.size()), nullptr, &m_pComputeShader);
			if (FAILED(hr)) return false;
			break;
		}
		return true;
	}


	///////////////////////////
	// Sets

	//Sets the shaders to the device context
	void Shader::SetShader(ID3D11DeviceContext* pContext)
	{
		switch (m_Type)
		{
		case ShaderType::Vertex:
			pContext->VSSetShader(m_pVertexShader, NULL, 0);
			pContext->IASetInputLayout(m_pLayout);
			break;
		case ShaderType::Pixel:
			pContext->PSSetShader(m_pPixelShader, NULL, 0);
			break;
		case ShaderType::Compute:
			pContext->CSSetShader(m_pComputeShader, NULL, 0);
			break;
		}
	}
}