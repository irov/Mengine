#	pragma once

#	include "Layer.h"
#	include "SceneNode2D.h"
#	include "Viewport.h"

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
		Viewport* getViewport();

	public:
		void loader( TiXmlElement * _xml ) override;

		bool handleKeyEvent( size_t _key, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( int _x, int _y, int _whell ) override;

	public:
		void setOffsetPosition( const mt::vec2f & _offset ) override;

	public:
		void update( float _timing ) override;

		void _addChildren(Node* _node);
		
	protected:
		bool _renderBegin() override;
		void _renderEnd() override;

		bool _activate() override;

	protected:
		void updateViewport();

	protected:
		mt::vec2f m_factorParallax;

		Viewport m_viewport;
	};
}