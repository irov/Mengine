#	pragma once

#	include "NodeImpl.h"

namespace Menge
{
	class Layer;

	class Scene
		: public NodeImpl
	{
		OBJECT_DECLARE(Scene);

	protected:
		bool addChildren(Node *_node) override;
	};
}