#include "Shaders\DepthShader.h"
#include <fstream>

namespace Render
{
	///////////////////////////
	// Construct / destruction

	//Sets all defaults
	DepthShader::DepthShader()
	{
		m_pLayout = NULL;
		m_pVertexShader = NULL;
		m_pPixelShader = NULL;
	}

	//Ensures cleanup of any DX stuff
	DepthShader::~DepthShader()
	{
		SAFE_RELEASE(m_pLayout);
		SAFE_RELEASE(m_pVertexShader);
		SAFE_RELEASE(m_pPixelShader);
	}

	//Initialises the shader and returns whether it was successful
	//Returns true if successful
	bool DepthShader::Init(ID3D11Device* pDevice, const std::string& vertexShader, const std::string& pixelShader)
	{
		std::vector<char> mByteCode;

		//Open compiled vertex shader file
		std::ifstream VSFile(vertexShader, std::ios::in | std::ios::binary | std::ios::ate);
		if (!VSFile.is_open()) return false;

		//Read in shader file
		std::streamoff fileSize = VSFile.tellg();
		VSFile.seekg(0, std::ios::beg);
		mByteCode.resize(static_cast<decltype(mByteCode.size())>(fileSize));
		VSFile.read(&mByteCode[0], fileSize);
		if (VSFile.fail()) return false;

		HRESULT hr = pDevice->CreateVertexShader(mByteCode.data(), static_cast<SIZE_T>(mByteCode.size()), nullptr, &m_pVertexShader);
		if (FAILED(hr)) return false;

		D3D11_INPUT_ELEMENT_DESC inputLayout[] =
		{
			// Semantic     Index  Format                        Slot  Offset  Slot Class                    Instance Step
			{ "POSITION",   0,     DXGI_FORMAT_R32G32B32_FLOAT,  0,    0,      D3D11_INPUT_PER_VERTEX_DATA,  0 },
			{ "NORMAL",     0,     DXGI_FORMAT_R32G32B32_FLOAT,  0,    12,     D3D11_INPUT_PER_VERTEX_DATA,  0 },
			{ "TEXCOORD",   0,     DXGI_FORMAT_R32G32_FLOAT,     0,    24,     D3D11_INPUT_PER_VERTEX_DATA,  0 }
		};
		pDevice->CreateInputLayout(inputLayout, 3, mByteCode.data(), static_cast<SIZE_T>(mByteCode.size()), &m_pLayout);

		//Open compiled pixel shader file
		std::ifstream PSFile(pixelShader, std::ios::in | std::ios::binary | std::ios::ate);
		if (!PSFile.is_open()) return false;

		//Read in shader file
		fileSize = PSFile.tellg();
		PSFile.seekg(0, std::ios::beg);
		mByteCode.clear();
		mByteCode.resize(static_cast<decltype(mByteCode.size())>(fileSize));
		PSFile.read(&mByteCode[0], fileSize);
		if (PSFile.fail()) return false;

		hr = pDevice->CreatePixelShader(mByteCode.data(), static_cast<SIZE_T>(mByteCode.size()), nullptr, &m_pPixelShader);
		if (FAILED(hr)) return false;

		return true;
	}


	///////////////////////////
	// Sets

	//Sets the shaders to the device context
	void DepthShader::SetShader(ID3D11DeviceContext* pContext)
	{
		pContext->PSSetShader(m_pPixelShader, NULL, 0);
		pContext->VSSetShader(m_pVertexShader, NULL, 0);
		pContext->IASetInputLayout(m_pLayout);
	}

	//Runs the shader
	void DepthShader::Run()
	{

	}
}