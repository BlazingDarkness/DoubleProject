#pragma once
#include "Manager.h"

namespace Scene
{
	///////////////////////////
	// Construct / destruction

	//Sets up initial scene
	Manager::Manager()
	{
		//nothing
	}

	//Destroys all scene objects
	Manager::~Manager()
	{
		for (auto light = m_LightList.begin(); light != m_LightList.end(); ++light)
		{
			delete (*light);
		}
		m_LightList.clear();
	}

	///////////////////////////
	// Scene creation

	//Creates a light with a colour and brightness with positional data from a matrix
	Light* Manager::CreateLight(const gen::CVector3& colour, const float brightness, const gen::CMatrix4x4& mat)
	{
		Light* light = new Light(colour, brightness, mat);

		m_LightList.push_back(light);

		return light;
	}

	//Removes the light from the scene
	void Manager::RemoveLight(Light* l)
	{
		for (auto light = m_LightList.begin(); light != m_LightList.end(); ++light)
		{
			if (l == *light)
			{
				m_LightList.erase(light);
				return;
			}
		}
	}
}