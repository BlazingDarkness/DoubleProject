#pragma once
#include "SceneNode.h"

namespace Scene
{
	class Light : public SceneNode
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Creates a light at the origin with a colour and brightness
		Light(const gen::CVector3& colour = Light::kWhite, const float brightness = 1.0f);

		//Creates a light with a colour, brightness, position, rotation, and scale
		Light(const gen::CVector3 colour = Light::kWhite,
			const float brightness = 1.0f,
			const gen::CVector3& pos = gen::CVector3::kOrigin,
			const gen::CVector3& rot = gen::CVector3::kZero,
			const gen::CVector3& scale = gen::CVector3::kOne);

		//Creates a light with a colour and brightness with positional data from a matrix
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