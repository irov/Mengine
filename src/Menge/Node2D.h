#	pragma once

#	include "Node.h"

namespace Menge
{
	class Node2D
		: public Node
	{
	public:
		Node2D();
		virtual ~Node2D(){};

	public:
		void invalidateWorldMatrix();
		void _invalidateWorldMatrix() override;

	/*	const mt::mat3f & getWorldMatrix();
		const mt::vec2f & getWorldPosition();
		const mt::vec2f & getWorldDirection();
*/
		mt::vec2f getScreenPosition();
	};
}	// namespace Menge
