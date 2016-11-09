#include "Rendering\TextureManager.h"
#include "DirectXTK\WICTextureLoader.h"

namespace Render
{
	///////////////////////////
	// Construct / destruction

	//Creates a texture manager
	TextureManager::TextureManager(ID3D11Device* pDevice)
	{
		m_pDevice = pDevice;
	}

	//Releases all textures
	TextureManager::~TextureManager()
	{
		for (auto itr = m_Textures.begin(); itr != m_Textures.end(); ++itr)
		{
			SAFE_RELEASE(itr->second.textureView);
			SAFE_RELEASE(itr->second.texture);
		}
		m_Textures.clear();
	}


	///////////////////////////
	// Gets & Sets

	//Attempts to load a texture from a file
	//Returns null if file could not be loaded
	//If the texture is already loaded it returns a pointer to the existing texture
	ID3D11ShaderResourceView* TextureManager::LoadTexture(const std::string& file)
	{
		//Check if texture has already beend loaded
		auto mapItr = m_Textures.find(file);

		if (mapItr != m_Textures.end())
		{
			++((*mapItr).second.count);
			return (*mapItr).second.textureView;
		}

		//Load texture

		ID3D11ShaderResourceView* textureView = NULL;
		ID3D11Resource* texture = NULL;
		std::wstring widestr = std::wstring(file.begin(), file.end());
		
		//DirectX::Create

		if (FAILED(DirectX::CreateWICTextureFromFile(m_pDevice, widestr.c_str(), &texture, &textureView)))//D3D11CreateShaderResourceViewFromFile(m_pDevice, widestr.c_str(), NULL, NULL, &resource, NULL)))
		{
			//Output error message with file name
#ifdef _DEBUG
			MessageBox(NULL, widestr.append(L"  Could not load texture file.").c_str(), L"Error", MB_OK);
#endif
			SAFE_RELEASE(texture);
			SAFE_RELEASE(textureView);
			return NULL;
		}
		else //Loaded successfully
		{
			m_Textures.insert(TexKeyPair{ file, Texture{ textureView, texture, 1 } });
			return textureView;
		}
	}

	//Decrements the number of objects using the texture
	//Deletes the texture if the count reaches zero
	void TextureManager::RemoveTexture(ID3D11ShaderResourceView* texture)
	{
		for (auto itr = m_Textures.begin(); itr != m_Textures.end(); ++itr)
		{
			//Search through map and check if same texture
			if (itr->second.textureView == texture)
			{
				//Decrement count of objects using texture, delete if zero
				--(itr->second.count);
				if (itr->second.count == 0)
				{
					SAFE_RELEASE(itr->second.textureView);
					SAFE_RELEASE(itr->second.texture);
					m_Textures.erase(itr);
				}
				return;
			}
		}
	}
}