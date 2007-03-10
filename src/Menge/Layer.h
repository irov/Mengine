#	pragma once

#	include "Renderable.h"

namespace Menge
{
	class Layer
		: public Renderable
	{
		OBJECT_DECLARE(Layer);

	public:
		Layer();

	public:
		void setParent(Node *node) override;

	public:
		const ViewPort & updateViewPort( const ViewPort & _viewPort ) override;

	protected:
		mt::mat3f m_matrixParalax;
		mt::vec2f m_factorParallax;

		ViewPort m_viewPort;
	};
}