#	pragma once

#	include "Kernel/Layer.h"
#	include "Core/Viewport.h"

namespace Menge
{
	class Camera2D;

	class Layer2D
		: public Layer
	{
        DECLARE_VISITABLE()

	public:
		Layer2D();

	public:
		void setParallaxFactor( const mt::vec2f & _factor );
		const mt::vec2f & getParallaxFactor() const;

	public:
		void setRenderViewport( const Viewport & _viewport );
		void removeRenderViewport();

    protected:
        void createRenderViewport_();
        void clearRenderViewport_();

	public:
		bool testBoundingBox( const Viewport & _viewport, const mt::box2f & _layerspaceBox, const mt::box2f & _screenspaceBox ) const override;

	public:
		mt::vec2f cameraToLocal( Camera2D * _camera2D, const mt::vec2f& _point );

		virtual void calcScreenPosition( mt::vec2f & _screen, const Viewport& _viewport, Node* _node ) const override;

    protected:
        bool _activate() override;
        void _deactivate() override;
    
    protected:
		void _addChildren( Node * _node ) override;

	protected:
		mt::vec2f m_factorParallax;

		Viewport m_viewport;
		
		Camera2D * m_cameraViewport;
		//Viewport m_viewportWM;
		//mt::mat4f m_viewMatrix;
		//mt::mat4f m_projectionMatrix;

		bool m_hasViewport;
	};
}
