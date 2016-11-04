#pragma once
#include "CVector3.h"
#include "CMatrix4x4.h"
#include <list>

namespace Scene
{
	class Node
	{
	public:

		///////////////////////////
		// Construct / destruction

		//Creates a node at the origin
		Node();

		//Creates a node with a position, rotation, and scale
		Node(const gen::CVector3& pos, const  gen::CVector3& rot = gen::CVector3::kZero, const gen::CVector3& scale = gen::CVector3::kOne);

		//Creates a node with position, rotation, and scale extracted from a matrix
		Node(const gen::CMatrix4x4& mat);

		//Destructor, ensures it is detached from all other nodes
		~Node();


		///////////////////////////
		// Getters & Setters

		//Note: All scales and positional data can be obtained through the matrix

		//Returns a reference to the relative matrix
		//If there is no parent node then this is equivalent to the world matrix
		gen::CMatrix4x4& Matrix()
		{
			return m_RelMatrix;
		}

		//Returns a reference to the world matrix
		//If there is no parent node then this is equivalent to the relative matrix
		gen::CMatrix4x4& WorldMatrix()
		{
			if (m_pParent)
			{
				UpdateWorldMatrix();
				return m_WorldMatrix;
			}
			else
			{
				return m_RelMatrix;
			}
		}

		//Sets the relative matrix
		//If there is no parent node then this is equivalent to the world matrix
		void SetMatrix(const gen::CMatrix4x4& mat)
		{
			m_RelMatrix = mat;
		}

		//Sets the parent node
		void SetParent(Node* node);


		///////////////////////////
		// Node relation management

		//The node is detached from the parent node and will
		//stay at the world position it was prior to be detached
		void DetachFromParent();

		//All child nodes are detached from this node
		void DetachFromChildren();

	private:
		///////////////////////////
		// Internal updates

		//Recalculates the world matrix based on the parent
		//node's matrix and this nodes relative matrix
		//If there is no parent node then world = rel
		void UpdateWorldMatrix();


		///////////////////////////
		// Node relation management

		//Used to tell a parent to detach a child
		//This function is called by the child on its parent
		void DetachChild(Node* child);

		//Used to tell a child to detach from its parent
		//This function is called by the parent on its child
		void DetachParent();

		//Adds a child node to this one
		void AddChild(Node* child);


		///////////////////////////
		// member variables

		gen::CMatrix4x4 m_WorldMatrix;
		gen::CMatrix4x4 m_RelMatrix;

		Node* m_pParent;
		std::list<Node*> m_pChildren;
	};
}