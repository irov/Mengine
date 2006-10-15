#	pragma once

#	include "Node.h"

#	include <list>

namespace Menge
{
	class Layer
		: public Node
	{
		OBJECT_DECLARE(Layer);

	public:
		void setParent(Node *node)override;

	};
}