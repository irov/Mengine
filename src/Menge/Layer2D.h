#	pragma once

#	include "Layer.h"
#	include "SceneNode2D.h"

#	include "Viewport.h"

#	include <list>

namespace Menge
{
	class Camera2D;

	class Layer2D
		: public Layer
		, public SceneNode2D
	{
		OBJECT_DECLARE(Layer2D);

	public:
		Layer2D();

	public:
		void setParallaxFactor( const mt::vec2f & _factor );

	public:
		const Viewport & updateViewport( const Viewport & _viewport ) override;

	public:
		void loader( TiXmlElement *_xml) override;
		void renderLayer() override;

	protected:
		mt::vec2f m_factorParallax;

		Viewport m_viewPort;
	};
}