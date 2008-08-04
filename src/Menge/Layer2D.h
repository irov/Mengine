#	pragma once

#	include "Layer.h"
#	include "Viewport.h"

namespace Menge
{
	class Camera2D;

	class Layer2D
		: public Layer
	{
		OBJECT_DECLARE(Layer2D);

	public:
		Layer2D();

	public:
		void setParallaxFactor( const mt::vec2f & _factor );
		const mt::vec2f & getParallaxFactor() const;

		Viewport getParallaxViewport( const Viewport & _viewport );

	public:
		void setOffsetPosition( const mt::vec2f & _offset ) override;

	public:
		void loader( XmlElement * _xml ) override;
		void render( unsigned int _debugMask ) override;

	protected:
		void _addChildren( Node * _node ) override;
		bool _activate() override;

	protected:
		mt::vec2f m_factorParallax;
	};
}