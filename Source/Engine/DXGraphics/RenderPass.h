#pragma once
#include "DXGraphics\IDXResource.h"
#include "DXGraphics\Shader.h"
#include <list>

namespace DXG
{
	class RenderPass
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Creates a render pass object that holds a number of shaders and resources
		RenderPass();

		///////////////////////////
		// Adds and Removes

		//Adds a shader to the render pass
		void AddShader(Shader* shader);

		//Adds a resource to be bounds during the render pass
		void AddResource(IDXResource* resource, ShaderType shaderType, uint index, BufferType bufferType);

		///////////////////////////
		// Bindings

		//Sets all shaders to the device context and binds all resources to them 
		void Bind(ID3D11DeviceContext* pDeviceContext);

		//Removes all shaders to the device context and unbinds all resources from them 
		void Unbind(ID3D11DeviceContext* pDeviceContext);

	private:
		///////////////////////////
		// data struct for resources

		struct ResourceMetaData
		{
			IDXResource* m_pResource;
			ShaderType m_ShaderType;
			uint m_Index;
			BufferType m_BufferType;
		};

		/////////////////////////////////////
		// Shaders and resources to be bounds

		std::list<Shader*> m_Shaders;
		std::list<ResourceMetaData> m_Resources;
	};
}