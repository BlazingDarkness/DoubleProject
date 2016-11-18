#pragma once
#include "Rendering\Mesh.h"
#include "Rendering\Material.h"
#include "Scene\Node.h"

namespace Scene
{
	class Model : public Node
	{
	public:
		///////////////////////////
		// Construct / destruction

		//Creates a model from a mesh at the origin
 		Model(Render::Mesh* pMesh);

		//Creates a model from a mesh with position, rotation, and scale
		Model(Render::Mesh* pMesh,
			const gen::CVector3& pos = gen::CVector3::kOrigin,
			const gen::CVector3& rot = gen::CVector3::kZero,
			const gen::CVector3& scale = gen::CVector3::kOne);

		//Creates a model from a mesh with positional data from a matrix
		Model(Render::Mesh* pMesh, const gen::CMatrix4x4& mat = gen::CMatrix4x4::kIdentity);


		///////////////////////////
		// Gets & Sets

		//Returns a pointer to the mesh
		Render::Mesh* GetMesh();

		//Returns a pointer to the material, this could potentially be null
		Render::Material* GetMaterial();

		//Changes the material that is applied to this model
		void SetMaterial(Render::Material* pMaterial);


	private:
		///////////////////////////
		// Variables

		Render::Material* m_pMaterial;
		Render::Mesh* m_pMesh;
	};
}