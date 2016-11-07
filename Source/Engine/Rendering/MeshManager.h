#pragma once
#include "Rendering\Mesh.h"
#include <unordered_map>

namespace Render
{
	class MeshManager
	{
	public:
	private:
		using MeshPair = std::pair<std::string, Mesh*>;
		using MeshMap = std::unordered_map<std::string, Mesh*>;

		MeshMap m_MeshMap;
	};
}