#	pragma once

#	include "Node.h"

namespace Menge
{
	class Node2D
		: public Node
		//, public Allocator2D
	{
	public:
		Node2D();
		virtual ~Node2D(){};

	public:
		void changePivot();
		void _changePivot() override;

		const mt::mat3f & getWorldMatrix();
		const mt::vec2f & getWorldPosition();
		const mt::vec2f & getWorldDirection();

		mt::vec2f getScreenPosition();
	};
}	// namespace Menge