#include "Scene\Model.h"

namespace Scene
{
	///////////////////////////
	// Construct / destruction

	//Creates a model from a mesh at the origin
	Model::Model(Render::Mesh* pMesh) : Node()
	{
		m_pMesh = pMesh;
		m_pMaterial = &Render::g_DefaultMaterial;
	}

	//Creates a model from a mesh with position, rotation, and scale
	Model::Model(Render::Mesh* pMesh,
		const gen::CVector3& pos, const gen::CVector3& rot, const gen::CVector3& scale) : Node(pos, rot, scale)
	{
		m_pMesh = pMesh;
		m_pMaterial = &Render::g_DefaultMaterial;
	}

	//Creates a model from a mesh with positional data from a matrix
	Model::Model(Render::Mesh* pMesh, const gen::CMatrix4x4& mat) : Node(mat)
	{
		m_pMesh = pMesh;
		m_pMaterial = &Render::g_DefaultMaterial;
	}


	///////////////////////////
	// Gets & Sets

	//Returns a pointer to the mesh
	Render::Mesh* Model::GetMesh()
	{
		return m_pMesh;
	}

	//Returns a pointer to the material, this could potentially be null
	Render::Material* Model::GetMaterial()
	{
		return m_pMaterial;
	}

	//Changes the material that is applied to this model
	void Model::SetMaterial(Render::Material* pMaterial)
	{
		m_pMaterial = pMaterial;
	}
}