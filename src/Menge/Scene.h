#	pragma once

#	include "Node.h"

namespace Menge
{
	class Layer;

	class Scene
		: public Node
	{
		OBJECT_DECLARE(Scene);

	protected:
		bool addChildren(Node *_node) override;
	};
}