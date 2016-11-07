#pragma once
#include "Rendering\IShader.h"

namespace Render
{
	class DepthShader : public IShader
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Sets all defaults
		DepthShader();

		//Ensures cleanup of any DX stuff
		~DepthShader();

		//Initialises the shader and returns whether it was successful
		virtual bool Init(ID3D11Device* pDevice);

		//Cleans up everything used by the shader
		virtual void Shutdown();

		virtual bool Render();

	private:
		ID3D11VertexShader* m_pVertexShader;
		ID3D11PixelShader* m_pPixelShader;
		ID3D11InputLayout* m_pLayout;
	};
}