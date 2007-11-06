#	pragma once

#	include "Node.h"
#	include "Resource.h"

namespace Menge
{
	class NodeResource
		: public virtual Node
		, public Resource
	{
	public:
		bool compile() override;
		void release() override;
		bool recompile() override;

		bool isCompile() override;
	};
}