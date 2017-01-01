#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class Layer
		: public Node
	{
		DECLARE_VISITABLE( Node )

	public:
		Layer();
		~Layer();
	};
}
