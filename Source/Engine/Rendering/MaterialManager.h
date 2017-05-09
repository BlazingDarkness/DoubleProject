#pragma once
#include "DXGraphics\DXIncludes.h"
#include "Rendering\TextureManager.h"
#include "Rendering\Material.h"

namespace Render
{
	class MaterialManager
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Creates a material manager from a texture manager
		MaterialManager(TextureManager* pTexManager);

		//Cleans up all texture stuff
		~MaterialManager();


		///////////////////////////
		// Material creation

		//Creates a material from a diffuse and specular texture
		Material* CreateMaterial(const std::string& texName, const std::string& diffuseTexFile, const std::string& specularTexFile);

		//Creates a material from a diffuse texture
		Material* CreateMaterial(const std::string& texName, const std::string& diffuseTexFile, float shinyness = 1.0f);

		//Creates a material from a colour
		Material* CreateMaterial(const std::string& texName, const gen::CVector4& diffuseColor, float shinyness = 1.0f);

		//Attempts to find the material from the list of existing materials
		//Returns a pointer to the default material if it doesn't exist
		Material* GetMaterial(const std::string& texName);

		///////////////////////////
		// Material destruction

		//Destroys a material
		void RemoveMaterial(Material* material);

	private:
		using MaterialList = std::list<Material*>;

		TextureManager* m_pTextureManager;
		MaterialList m_MaterialList;
	};
}