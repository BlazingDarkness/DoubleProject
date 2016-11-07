#pragma once
#include "DXIncludes.h"
#include "CVector3.h"

namespace Render
{
	struct Material
	{
		gen::CVector3 m_Colour;
		bool m_HasTexture;
		bool m_HasTransparency;
		ID3D11ShaderResourceView* m_pTexture;
	};
}