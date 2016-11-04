#include "Node.h"

namespace Scene
{
	///////////////////////////
	// Construct / destruction

	//Creates a node at the origin
	Node::Node()
	{
		m_RelMatrix.MakeIdentity();
		m_pParent = nullptr;
	}

	//Creates a node with a position, rotation, and scale
	Node::Node(const gen::CVector3& pos, const  gen::CVector3& rot, const gen::CVector3& scale)
	{
		m_RelMatrix = gen::CMatrix4x4(pos, rot, gen::kZXY, scale);
		m_pParent = nullptr;
	}

	//Creates a node with position, rotation, and scale extracted from a matrix
	Node::Node(const gen::CMatrix4x4& mat)
	{
		m_RelMatrix = mat;
		m_pParent = nullptr;
	}

	//Destructor, ensures it is detached from all other nodes
	Node::~Node()
	{
		if (m_pParent) DetachFromParent();
		if (m_pChildren.size() > 0) DetachFromChildren();
	}


	///////////////////////////
	// Setters

	//Sets the parent node
	void Node::SetParent(Node* node)
	{
		//Removes any existing parent
		if (m_pParent) DetachFromParent();

		m_pParent = node;
		node->AddChild(this);
	}


	///////////////////////////
	// Node relation management

	//The node is detached from the parent node and will
	//stay at the world position it was prior to be detached
	void Node::DetachFromParent()
	{
		//Exit early if no parent to detach from
		if (m_pParent == nullptr) return;

		//Ensure same global position before and after detachment
		UpdateWorldMatrix();
		m_RelMatrix = m_WorldMatrix;

		m_pParent->DetachChild(this);
		m_pParent = nullptr;
	}

	void Node::DetachFromChildren()
	{
		for (auto child = m_pChildren.begin(); child != m_pChildren.end(); ++child)
		{
			(*child)->DetachParent();
		}
		m_pChildren.clear();
	}

	///////////////////////////
	// Internal updates

	//Recalculates the world matrix based on the parent
	//node's matrix and this nodes relative matrix
	//If there is no parent node then world = rel
	void Node::UpdateWorldMatrix()
	{
		if (m_pParent != nullptr)
		{
			m_WorldMatrix = m_pParent->WorldMatrix() * m_RelMatrix;
		}
		else
		{
			m_WorldMatrix = m_RelMatrix;
		}
	}

	//Used to tell a parent to detach a child
	//This function is called by the child on its parent
	void Node::DetachChild(Node* child)
	{
		auto itr = m_pChildren.begin();
		while (itr != m_pChildren.end())
		{
			if (child == (*itr))
			{
				m_pChildren.erase(itr);
				return;
			}
		}
	}

	//Used to tell a child to detach from its parent
	//This function is called by the parent on its child
	void Node::DetachParent()
	{
		//Ensure same global position before and after detachment
		UpdateWorldMatrix();
		m_RelMatrix = m_WorldMatrix;

		m_pParent = nullptr;
	}

	//Adds a child node to this one
	void Node::AddChild(Node* child)
	{
		m_pChildren.push_back(child);
	}

	/*gen::CMatrix4x4 m_WorldMatrix;
	gen::CMatrix4x4 m_RelMatrix;

	Node* m_pParent;
	std::list<Node*> m_pChildren;*/
}