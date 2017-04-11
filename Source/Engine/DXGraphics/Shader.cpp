#pragma once
#include "DXGraphics\Shader.h"
#include <fstream>
#include <d3dcompiler.h>

namespace DXG
{
	//Function obtained from https://takinginitiative.wordpress.com/2011/12/11/directx-1011-basic-shader-reflection-automatic-input-layout-creation/
	HRESULT CreateInputLayoutDescFromVertexShaderSignature(ID3DBlob* pShaderBlob, ID3D11Device* pD3DDevice, ID3D11InputLayout** pInputLayout)
	{
		// Reflect shader info
		ID3D11ShaderReflection* pVertexShaderReflection = NULL;
		if (FAILED(D3DReflect(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection)))
		{
			return S_FALSE;
		}

		// Get shader info
		D3D11_SHADER_DESC shaderDesc;
		pVertexShaderReflection->GetDesc(&shaderDesc);

		// Read input layout description from shader info
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
		for (uint i = 0; i< shaderDesc.InputParameters; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
			pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

			// fill out input element desc
			D3D11_INPUT_ELEMENT_DESC elementDesc;
			elementDesc.SemanticName = paramDesc.SemanticName;
			elementDesc.SemanticIndex = paramDesc.SemanticIndex;
			elementDesc.InputSlot = 0;
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			// determine DXGI format
			if (paramDesc.Mask == 1)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (paramDesc.Mask <= 3)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (paramDesc.Mask <= 7)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (paramDesc.Mask <= 15)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			//save element desc
			inputLayoutDesc.push_back(elementDesc);
		}

		// Try to create Input Layout
		HRESULT hr = pD3DDevice->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.size(), pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pInputLayout);

		//Free allocation shader reflection memory
		pVertexShaderReflection->Release();
		return hr;
	}


	///////////////////////////
	// Construct / destruction

	//Sets all defaults
	Shader::Shader()
	{
		m_pLayout = NULL;
		m_pVertexShader = NULL;
		m_pShaderBlob = NULL;
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
		SAFE_RELEASE(m_pShaderBlob);
	}

	//Initialises the shader and returns whether it was successful
	bool Shader::Init(ID3D11Device* pDevice, const ShaderType type, const std::string& shaderFile)
	{
		HRESULT hr;
		m_Type = type;

		//Open compiled vertex shader file
		std::ifstream ShaderFile(shaderFile, std::ios::in | std::ios::binary | std::ios::ate);
		if (!ShaderFile.is_open()) return false;


		//Read in shader file
		std::streamoff fileSize = ShaderFile.tellg();
		D3DCreateBlob(static_cast<SIZE_T>(fileSize), &m_pShaderBlob);
		ShaderFile.seekg(0, std::ios::beg);
		ShaderFile.read(static_cast<char*>(m_pShaderBlob->GetBufferPointer()), fileSize);
		if (ShaderFile.fail()) return false;

		switch (type)
		{
		case ShaderType::Vertex:
			hr = pDevice->CreateVertexShader(m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pVertexShader);
			if (FAILED(hr)) return false;
			hr = CreateInputLayoutDescFromVertexShaderSignature(m_pShaderBlob, pDevice, &m_pLayout);
			if (FAILED(hr)) return false;
			break;
		case ShaderType::Hull:
			hr = pDevice->CreateHullShader(m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pHullShader);
			if (FAILED(hr)) return false;
			break;

		case ShaderType::Domain:
			hr = pDevice->CreateDomainShader(m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pDomainShader);
			if (FAILED(hr)) return false;
			break;

		case ShaderType::Geometry:
			hr = pDevice->CreateGeometryShader(m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pGeometryShader);
			if (FAILED(hr)) return false;
			break;

		case ShaderType::Pixel:
			hr = pDevice->CreatePixelShader(m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pPixelShader);
			if (FAILED(hr)) return false;
			break;

		case ShaderType::Compute:
			hr = pDevice->CreateComputeShader(m_pShaderBlob->GetBufferPointer(), m_pShaderBlob->GetBufferSize(), nullptr, &m_pComputeShader);
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

	//Unbinds the shader from the pipeline
	void Shader::Unbind(ID3D11DeviceContext* pContext)
	{
		switch (m_Type)
		{
		case ShaderType::Vertex:
			pContext->VSSetShader(NULL, NULL, 0);
			break;
		case ShaderType::Hull:
			pContext->HSSetShader(NULL, NULL, 0);
			break;
		case ShaderType::Domain:
			pContext->DSSetShader(NULL, NULL, 0);
			break;
		case ShaderType::Geometry:
			pContext->GSSetShader(NULL, NULL, 0);
			break;
		case ShaderType::Pixel:
			pContext->PSSetShader(NULL, NULL, 0);
			break;
		case ShaderType::Compute:
			pContext->CSSetShader(NULL, NULL, 0);
			break;
		}
	}
}