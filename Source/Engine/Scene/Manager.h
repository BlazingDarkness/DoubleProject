#pragma once
#include "Light.h"
#include <list>

namespace Scene
{
	class Manager
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Sets up initial scene
		Manager();

		//Destroys all scene objects
		~Manager();

		///////////////////////////
		// Scene creation

		//Creates a light with a colour and brightness with positional data from a matrix
		Light* CreateLight(const gen::CVector3& colour = Light::kWhite, const float brightness = 1.0f, const gen::CMatrix4x4& mat = gen::CMatrix4x4::kIdentity);

		//Removes the light from the scene
		void RemoveLight(Light* l);

	private:
		std::list<Light*> m_LightList;
	};
}