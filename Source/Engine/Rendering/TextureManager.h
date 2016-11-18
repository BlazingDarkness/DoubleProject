#pragma once
#include "Rendering\DXIncludes.h"
#include <unordered_map>
#include <string>

namespace Render
{
	//This class is adapted from a DX10 version used in a prior assignment

	class TextureManager
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Creates a texture manager
		TextureManager(ID3D11Device* pDevice);

		//Releases all textures
		~TextureManager();


		///////////////////////////
		// Gets & Sets

		//Attempts to load a texture from a file
		//Returns null if file could not be loaded
		//If the texture is already loaded it returns a pointer to the existing texture
		ID3D11ShaderResourceView* LoadTexture(const std::string& file);

		//Decrements the number of objects using the texture
		//Deletes the texture if the count reaches zero
		void RemoveTexture(ID3D11ShaderResourceView* texture);

	private:
		///////////////////////////
		// type defs

		struct Texture { ID3D11ShaderResourceView* textureView; ID3D11Resource* texture; int count; };
		using TextureMap = std::unordered_map<std::string, Texture>;
		using TexKeyPair = std::pair<std::string, Texture>;


		///////////////////////////
		// member variables

		TextureMap m_Textures;
		ID3D11Device* m_pDevice;
	};
}