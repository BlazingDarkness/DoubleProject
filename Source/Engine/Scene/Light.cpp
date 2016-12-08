#pragma once
#include "Light.h"
using namespace gen;

namespace Scene
{
	///////////////////////////
	// Construct / destruction

	//Creates a light with a colour and brightness with positional data from a matrix (default world origin)
	Light::Light(const gen::CVector3& colour, const float brightness, const gen::CMatrix4x4& mat) : Node(mat)
	{
		m_Colour = colour;
		m_Brightness = brightness;
	}


	///////////////////////////
	// static constants

	const CVector3 Light::kWhite	= CVector3(1.0f, 1.0f, 1.0f);
	const gen::CVector3 Light::kRed		= CVector3(1.0f, 0.0f, 0.0f);
	const gen::CVector3 Light::kOrange	= CVector3(1.0f, 0.4f, 0.0f);
	const gen::CVector3 Light::kYellow	= CVector3(1.0f, 1.0f, 0.0f);
	const gen::CVector3 Light::kGreen	= CVector3(0.0f, 1.0f, 0.0f);
	const gen::CVector3 Light::kCyan	= CVector3(0.0f, 1.0f, 1.0f);
	const gen::CVector3 Light::kBlue	= CVector3(0.0f, 0.0f, 1.0f);
	const gen::CVector3 Light::kMagenta = CVector3(1.0f, 0.0f, 1.0f);
}