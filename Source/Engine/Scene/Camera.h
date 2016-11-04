#pragma once
#include "Node.h"

namespace Scene
{
	class Camera : public Node
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Creates a camera at the origin
		Camera(const float FOV = 90, const float nearClip = 1.0f, const float farClip = 10000.0f);

		//Creates a camera with positional data from a matrix
		Camera(const float FOV = 90, const float nearClip = 1.0f, const float farClip = 10000.0f, const gen::CMatrix4x4& mat = gen::CMatrix4x4::kIdentity);

		//Destructor
		~Camera() {}


		///////////////////////////
		// Getters & Setters

		gen::CMatrix4x4 GetViewMatrix();

		gen::CMatrix4x4 GetProjectionMatrix();
		
		gen::CMatrix4x4 GetViewProjectionMatrix();

		float GetFOV() { return m_FOV; }

		void SetFOV(const float fov) { m_FOV = fov; }

		float GetNearClip() { return m_NearClip; }

		void SetNearClip(const float nearClip) { m_NearClip = nearClip; }

		float GetFarClip() { return m_FarClip; }

		void SetFarClip(const float farClip) { m_FarClip = farClip; }

	private:
		///////////////////////////
		// member variables

		float m_FOV;
		float m_NearClip;
		float m_FarClip;

		gen::CMatrix4x4 m_ViewMatrix;
		gen::CMatrix4x4 m_ProjMatrix;
		gen::CMatrix4x4 m_ViewProjMatrix;
	};
}