#	pragma once

#	include "Allocator2D.h"

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

	public:
		const mt::mat3f & getWorldMatrix() override;

	protected:
		mt::mat3f m_matrixParalax;
		mt::vec2f m_factorParallax;
	};
}