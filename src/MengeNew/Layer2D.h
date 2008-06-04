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

		void setScrollable( bool _scrollable );
		bool isScrollable() const;


		Viewport getParallaxViewport( const Viewport & _viewport );
		//const mt::vec2f& getViewportOffset() const;
		//mt::vec2f screenToLocal( const mt::vec2f& _point );

	public:
		void loader( XmlElement * _xml ) override;

	public:
		void setOffsetPosition( const mt::vec2f & _offset ) override;
		//void setRenderTarget( const std::string& _cameraName ) override;
		//bool needReRender();

	public:
		void _update( float _timing ) override;
		void render( const Viewport & _viewport ) override;

		void _addChildren( Node * _node ) override;
		
	protected:
		void _debugRender( const Viewport & _viewport ) override;

		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;

	protected:
		void updateViewport();

	protected:
		mt::vec2f m_factorParallax;
		bool m_scrollable;
	};
}