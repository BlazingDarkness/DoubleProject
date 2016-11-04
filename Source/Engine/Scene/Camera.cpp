#include "Camera.h"

namespace Scene
{
	///////////////////////////
	// Construct / destruction

	//Creates a camera at the origin
	Camera::Camera(const float FOV, const float nearClip, const float farClip) : Node()
	{
		m_FOV = FOV;
		m_NearClip = nearClip;
		m_FarClip = farClip;
	}

	//Creates a camera with positional data from a matrix
	Camera::Camera(const float FOV, const float nearClip, const float farClip, const gen::CMatrix4x4& mat) : Node(mat)
	{
		m_FOV = FOV;
		m_NearClip = nearClip;
		m_FarClip = farClip;
	}

	gen::CMatrix4x4 Camera::GetViewMatrix()
	{
		m_ViewMatrix = WorldMatrix();
		m_ViewMatrix.Invert();

		return m_ViewMatrix;
	}

	gen::CMatrix4x4 Camera::GetProjectionMatrix()
	{
		//To do
	}

	gen::CMatrix4x4 Camera::GetViewProjectionMatrix()
	{
		//To do
	}
}
