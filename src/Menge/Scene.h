#	pragma once

#	include "Node.h"

#	include <list>

namespace Menge
{
	class Layer;

	typedef std::list<Layer *> TListLayer;

	class Scene
		: public Node
	{
		OBJECT_DECLARE(Scene);

	public:
		void start();

	protected:
		bool addChildren(Node *_node) override;
	};
}