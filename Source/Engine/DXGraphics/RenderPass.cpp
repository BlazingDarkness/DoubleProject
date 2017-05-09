#include "DXGraphics\RenderPass.h"

namespace DXG
{
	///////////////////////////
	// Construct / destruction

	//Creates a render pass object that holds a number of shaders and resources
	RenderPass::RenderPass()
	{

	}

	///////////////////////////
	// Adds and Removes

	//Adds a shader to the render pass
	void RenderPass::AddShader(Shader* shader)
	{
		m_Shaders.push_back(shader);
	}

	//Adds a resource to be bounds during the render pass
	void RenderPass::AddResource(IDXResource* resource, ShaderType shaderType, uint index, BufferType bufferType)
	{
		ResourceMetaData metaData = { resource, shaderType, index, bufferType };

		m_Resources.push_back(metaData);
	}

	//Removes a resource
	void RenderPass::RemoveResource(IDXResource* resource)
	{
		for (auto itr = m_Resources.begin(); itr != m_Resources.end(); ++itr)
		{
			if ((*itr).m_pResource == resource)
			{
				m_Resources.erase(itr);
				return;
			}
		}
	}

	///////////////////////////
	// Bindings

	//Sets all shaders to the device context and binds all resources to them 
	void RenderPass::Bind(ID3D11DeviceContext* pDeviceContext)
	{
		for (auto itr = m_Shaders.begin(); itr != m_Shaders.end(); ++itr)
		{
			(*itr)->SetShader(pDeviceContext);
		}

		for (auto itr = m_Resources.begin(); itr != m_Resources.end(); ++itr)
		{
			auto& data = (*itr);
			data.m_pResource->Bind(pDeviceContext, data.m_ShaderType, data.m_Index, data.m_BufferType);
		}
	}

	//Removes all shaders to the device context and unbinds all resources from them 
	void RenderPass::Unbind(ID3D11DeviceContext* pDeviceContext)
	{
		for (auto itr = m_Resources.begin(); itr != m_Resources.end(); ++itr)
		{
			auto& data = (*itr);
			data.m_pResource->Unbind(pDeviceContext, data.m_ShaderType, data.m_Index, data.m_BufferType);
		}

		for (auto itr = m_Shaders.begin(); itr != m_Shaders.end(); ++itr)
		{
			(*itr)->Unbind(pDeviceContext);
		}
	}
}