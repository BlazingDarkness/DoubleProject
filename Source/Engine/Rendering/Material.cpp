#include "Rendering\Material.h"

namespace Render
{
	///////////////////////////
	// Construct / destruction

	//Creates a material from a diffuse and specular texture
	Material::Material(const std::string& name, ID3D11ShaderResourceView* pDiffuseTex, ID3D11ShaderResourceView* pSpecularTex)
	{
		//Material Properties
		m_Name = name;
		m_Alpha = 1.0f;
		m_Dirtyness = 0.0f;
		m_Shinyness = 1.0f;
		m_DiffuseColor = {0.7f, 0.7f, 0.7f, 1.0f};

		//Texture properties
		m_pDiffuseTex = pDiffuseTex;
		m_pSpecularTex = pSpecularTex;

		//settings
		m_HasDirt = false;
		m_HasAlpha = false;
		m_HasDifTex = true;
		m_HasSpecTex = true;
	}

	//Creates a material from a colour
	Material::Material(const std::string& name, const gen::CVector4& diffuseColor, float shinyness)
	{
		//Material Properties
		m_Name = name;
		m_Alpha = 1.0f;
		m_Dirtyness = 0.0f;
		m_Shinyness = shinyness;
		m_DiffuseColor = diffuseColor;

		//Texture properties
		m_pDiffuseTex = NULL;
		m_pSpecularTex = NULL;

		//settings
		m_HasDirt = false;
		m_HasAlpha = false;
		m_HasDifTex = false;
		m_HasSpecTex = false;
	}


}