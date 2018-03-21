#pragma once

#include "fastpathfinder/graph.h"

#include "Kernel/Scriptable.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class PathGraphNode
		: public fastpathfinder::graph_node
		, public Scriptable
	{
	public:
		uint32_t getWeight() const
		{
			return this->weight;
		}
	};
}

