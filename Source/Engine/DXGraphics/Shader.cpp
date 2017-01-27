#pragma once
#include "DXGraphics\Shader.h"
#include <fstream>

namespace DXG
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
		switch (m_Type)
		{
		case ShaderType::Vertex:
			SAFE_RELEASE(m_pLayout);
			SAFE_RELEASE(m_pVertexShader);
			break;
		case ShaderType::Hull:
			SAFE_RELEASE(m_pHullShader);
			break;
		case ShaderType::Domain:
			SAFE_RELEASE(m_pDomainShader);
			break;
		case ShaderType::Geometry:
			SAFE_RELEASE(m_pGeometryShader);
			break;
		case ShaderType::Pixel:
			SAFE_RELEASE(m_pPixelShader);
			break;
		case ShaderType::Compute:
			SAFE_RELEASE(m_pComputeShader);
			break;
		}
	}

	//Initialises the shader and returns whether it was successful
	bool Shader::Init(ID3D11Device* pDevice, const ShaderType type, const std::string& shaderFile)
	{
		HRESULT hr;
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
		{
			hr = pDevice->CreateVertexShader(byteCode.data(), static_cast<SIZE_T>(byteCode.size()), nullptr, &m_pVertexShader);
			if (FAILED(hr)) return false;

			D3D11_INPUT_ELEMENT_DESC inputLayout[] =
			{
				// Semantic     Index  Format                        Slot  Offset  Slot Class                    Instance Step
				{ "POSITION",   0,     DXGI_FORMAT_R32G32B32_FLOAT,  0,    0,      D3D11_INPUT_PER_VERTEX_DATA,  0 },
				{ "NORMAL",     0,     DXGI_FORMAT_R32G32B32_FLOAT,  0,    12,     D3D11_INPUT_PER_VERTEX_DATA,  0 },
				{ "TEXCOORD",   0,     DXGI_FORMAT_R32G32_FLOAT,     0,    24,     D3D11_INPUT_PER_VERTEX_DATA,  0 }
			};
			hr = pDevice->CreateInputLayout(inputLayout, 3, byteCode.data(), static_cast<SIZE_T>(byteCode.size()), &m_pLayout);
			if (FAILED(hr)) return false;
		}
			break;
		case ShaderType::Hull:
			hr = pDevice->CreateHullShader(byteCode.data(), static_cast<SIZE_T>(byteCode.size()), nullptr, &m_pHullShader);
			if (FAILED(hr)) return false;
			break;

		case ShaderType::Domain:
			hr = pDevice->CreateDomainShader(byteCode.data(), static_cast<SIZE_T>(byteCode.size()), nullptr, &m_pDomainShader);
			if (FAILED(hr)) return false;
			break;

		case ShaderType::Geometry:
			hr = pDevice->CreateGeometryShader(byteCode.data(), static_cast<SIZE_T>(byteCode.size()), nullptr, &m_pGeometryShader);
			if (FAILED(hr)) return false;
			break;

		case ShaderType::Pixel:
			hr = pDevice->CreatePixelShader(byteCode.data(), static_cast<SIZE_T>(byteCode.size()), nullptr, &m_pPixelShader);
			if (FAILED(hr)) return false;
			break;

		case ShaderType::Compute:
			hr = pDevice->CreateComputeShader(byteCode.data(), static_cast<SIZE_T>(byteCode.size()), nullptr, &m_pComputeShader);
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
		case ShaderType::Hull:
			pContext->HSSetShader(m_pHullShader, NULL, 0);
			break;
		case ShaderType::Domain:
			pContext->DSSetShader(m_pDomainShader, NULL, 0);
			break;
		case ShaderType::Geometry:
			pContext->GSSetShader(m_pGeometryShader, NULL, 0);
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