#pragma once

#include "fastpathfinder/graph.h"

#include "Kernel/Scriptable.h"
#include "Factory/Factorable.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class PathGraphNode
		: public Factorable
		, public Scriptable
        , public fastpathfinder::graph_node
	{
	public:
		uint32_t getWeight() const
		{
			return this->weight;
		}
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PathGraphNode> PathGraphNodePtr;
    //////////////////////////////////////////////////////////////////////////
}

