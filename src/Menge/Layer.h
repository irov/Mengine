#	pragma once

#	include "Allocator2D.h"

#	include <list>

namespace Menge
{
	class Layer
		: public Allocator2D
	{
		OBJECT_DECLARE(Layer);

	public:
		Layer();

	public:
		void setParent(Node *node)override;

	protected:
		bool _updateMatrix() override;

	protected:
		mt::vec2f m_factorParallax;
	};
}