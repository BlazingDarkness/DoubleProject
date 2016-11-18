#pragma once
#include "Rendering\DXIncludes.h"
#include "Rendering\TextureManager.h"
#include "CVector4.h"

namespace Render
{
	class Material
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Creates a material from a diffuse and specular texture
		Material(const std::string& name, ID3D11ShaderResourceView* pDiffuseTex, ID3D11ShaderResourceView* pSpecularTex);

		//Creates a material from a colour
		Material(const std::string& name, const gen::CVector4& diffuseColor, float shinyness = 1.0f);


		///////////////////////////
		// Gets

		const std::string& GetName() { return m_Name; }

		float GetAlpha() { return m_Alpha; }
		float GetDirtyness() { return m_Dirtyness; }
		float GetShinyness() { return m_Shinyness; }
		const gen::CVector4& GetDiffuseColour() { return m_DiffuseColor; }

		bool HasDirt() { return m_HasDirt; }
		bool HasAlpha() { return m_HasAlpha; }
		bool HasDiffuseTex() { return m_HasDifTex; }
		bool HasSpecularTex() { return m_HasSpecTex; }

	private:
		//Material Properties
		std::string m_Name;
		float m_Alpha;//Range of 0.0f to 1.0f
		float m_Dirtyness;//Range of 0.0f to 1.0f
		float m_Shinyness;//Range of 0.0f to 1.0f
		gen::CVector4 m_DiffuseColor;

		//Texture properties
		ID3D11ShaderResourceView* m_pDiffuseTex;
		ID3D11ShaderResourceView* m_pSpecularTex;

		//settings
		bool m_HasDirt;
		bool m_HasAlpha;
		bool m_HasDifTex;
		bool m_HasSpecTex;
	} g_DefaultMaterial = { std::string(""), {0.7f, 0.7f, 0.7f, 1.0f}, 1.0f };
}