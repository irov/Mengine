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
		void setParallaxFactor( const mt::vec2f & _factor );

	public:
		void setParent(Node *node) override;

	public:
		const Viewport & updateViewport( const Viewport & _viewPort ) override;

	public:
		void loader( TiXmlElement *_xml) override;

	protected:
		mt::vec2f m_factorParallax;

		Viewport m_viewPort;
	};
}