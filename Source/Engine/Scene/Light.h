#pragma once
#include "Node.h"

namespace Scene
{
	class Light : public Node
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Creates a light with a colour and brightness with positional data from a matrix (default world origin)
		Light(const gen::CVector3& colour = Light::kWhite, const float brightness = 1.0f, const gen::CMatrix4x4& mat = gen::CMatrix4x4::kIdentity);

		//Destructor
		~Light() {}


		///////////////////////////
		// Getters & Setters

		//Returns a copy of the light's colour;
		gen::CVector3 GetColour()
		{
			return m_Colour;
		}

		//Sets the light's colour
		void SetColour(const gen::CVector3& colour)
		{
			m_Colour = colour;
		}

		//Returns the brightness
		float GetBrightness()
		{
			return m_Brightness;
		}

		//Sets the brightness
		void SetBrightness(const float brightness)
		{
			m_Brightness = brightness;
		}


		///////////////////////////
		// static constants

		static const gen::CVector3 kWhite;
		static const gen::CVector3 kRed;
		static const gen::CVector3 kOrange;
		static const gen::CVector3 kYellow;
		static const gen::CVector3 kGreen;
		static const gen::CVector3 kCyan;
		static const gen::CVector3 kBlue;
		static const gen::CVector3 kMagenta;

	private:
		///////////////////////////
		// member variables

		gen::CVector3 m_Colour;
		float m_Brightness;
	};
}