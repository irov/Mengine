#	pragma once

#	include "NodeImpl.h"

#	include <list>

namespace Menge
{
	class Layer
		: public NodeImpl
	{
		OBJECT_DECLARE(Layer);

	public:
		void setParent(Node *node)override;

	};
}