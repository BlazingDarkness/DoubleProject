#include "Rendering\MaterialManager.h"

namespace Render
{
	///////////////////////////
	// Construct / destruction

	//Creates a material manager from a texture manager
	MaterialManager::MaterialManager(TextureManager* pTexManager)
	{
		m_pTextureManager = pTexManager;
	}

	//Cleans up all texture stuff
	MaterialManager::~MaterialManager()
	{
		for (auto material = m_MaterialList.begin(); material != m_MaterialList.end(); ++material)
		{
			delete (*material);
		}
	}


	///////////////////////////
	// Material creation
	
	//Creates a material from a diffuse and specular texture
	//Returns a pointer to the default material if unable to load textures
	Material* MaterialManager::CreateMaterial(const std::string& texName, const std::string& diffuseTexFile, const std::string& specularTexFile)
	{
		ID3D11ShaderResourceView* pDiffuseTex = m_pTextureManager->LoadTexture(diffuseTexFile);
		if (pDiffuseTex == NULL) return &g_DefaultMaterial;

		ID3D11ShaderResourceView* pSpecularTex = m_pTextureManager->LoadTexture(diffuseTexFile);
		if (pSpecularTex == NULL)
		{
			m_pTextureManager->RemoveTexture(pDiffuseTex);
			return &g_DefaultMaterial;
		}

		Material* m = new Material(texName, pDiffuseTex, pSpecularTex);

		m_MaterialList.push_back(m);

		return m;
	}

	//Creates a material from a colour
	Material* MaterialManager::CreateMaterial(const std::string& texName, const gen::CVector4& diffuseColor, float shinyness)
	{
		Material* m = new Material(texName, diffuseColor, shinyness);

		m_MaterialList.push_back(m);

		return m;
	}

	//Attempts to find the material from the list of existing materials
	//Returns a pointer to the default material if it doesn't exist
	Material* MaterialManager::GetMaterial(const std::string& texName)
	{
		for (auto material = m_MaterialList.begin(); material != m_MaterialList.end(); ++material)
		{
			if ((*material)->GetName() == texName)
			{
				return (*material);
			}
		}

		return &g_DefaultMaterial;
	}
}