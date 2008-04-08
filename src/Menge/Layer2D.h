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
		const mt::vec2f& getViewportOffset() const;
		bool isScrollable();
		mt::vec2f screenToLocal( const mt::vec2f& _point );

	public:
		void loader( XmlElement * _xml ) override;

		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;

	public:
		void setOffsetPosition( const mt::vec2f & _offset ) override;
		void setRenderTarget( const std::string& _cameraName ) override;
		bool needReRender();

	public:
		void update( float _timing ) override;
		void render() override;

		void _addChildren( SceneNode2D * _node ) override;
		
	protected:
		bool _renderBegin() override;
		void _render() override;
		void _renderEnd() override;

		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;

	protected:
		void updateViewport();

	protected:
		mt::vec2f m_factorParallax;

		Viewport m_viewport;
		bool m_needReRender;
		bool m_reRender;
		bool m_scrollable;
		mt::vec2f m_viewportOffset;
	};
}