#	pragma once

#	include "Layer.h"
#	include "Core/Viewport.h"

namespace Menge
{
	class Camera2D;

	class Layer2D
		: public Layer
	{
	public:
		Layer2D();

	public:
		void setParallaxFactor( const mt::vec2f & _factor );
		const mt::vec2f & getParallaxFactor() const;

		void setRenderViewport( const Viewport & _viewport );
		const Viewport & getRenderViewport() const;

		bool testBoundingBox( const Viewport & _viewport, const mt::box2f & _layerspaceBox, const mt::box2f & _screenspaceBox ) const override;

	public:
		mt::vec2f screenToLocal( const mt::vec2f& _point );

		virtual mt::vec2f calcScreenPosition( const Viewport& _viewport, Node* _node ) const override;

		Camera* getCamera() override;

	public:
		void loader( XmlElement * _xml ) override;
		void render( Camera2D * _camera ) override;

	protected:
		void _addChildren( Node * _node ) override;
		bool _activate() override;
		void _deactivate() override;

	protected:
		mt::vec2f m_factorParallax;
		Viewport m_renderViewport;

		Camera2D* m_camera2D;
	};
}
