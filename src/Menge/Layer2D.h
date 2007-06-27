#	pragma once

#	include "SceneNode2D.h"

namespace Menge
{
	class Layer2D
		: public SceneNode2D
	{
		OBJECT_DECLARE(Layer2D);

	public:
		Layer2D();

	public:
		void setParallaxFactor( const mt::vec2f & _factor );

	public:
		const Viewport & updateViewport( const Viewport & _viewPort ) override;

	public:
		void loader( TiXmlElement *_xml) override;

	protected:
		mt::vec2f m_factorParallax;

		Viewport m_viewPort;
	};
}