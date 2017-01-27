#pragma once
#include "DXGraphics\DXIncludes.h"
#include "CVector4.h"
#include "CMatrix4x4.h"
#include <string>
#include <vector>

namespace Render
{
	class ModelShader
	{
	public:
		struct GlobalLightData
		{
			gen::CVector4 AmbientColour;
			gen::CVector4 CameraPos;
			float SpecularPower;
			unsigned int NumOfLights;
			gen::CVector2 Padding;
		};

		///////////////////////////
		// Construct / destruction

		//Sets all defaults
		ModelShader();

		//Ensures cleanup of any DX stuff
		~ModelShader();

		//Initialises the shader and returns whether it was successful
		bool Init(ID3D11Device* pDevice, const std::string& vertexShader, const std::string& pixelShader);

		//Sets the shaders to the device context
		void SetShader(ID3D11DeviceContext* pContext);

		//Runs the shader
		void Run();


		///////////////////////////
		// Gets & Sets

		UINT        ByteCodeLength() const { return static_cast<UINT>(mByteCode.size()); }
		const void* ByteCode()       const { return mByteCode.data(); }

	private:
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;
		ID3D11InputLayout* m_pLayout;

		std::vector<char> mByteCode;
	};
}