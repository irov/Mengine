#	pragma once

#	include "Kernel/Layer.h"
#	include "Core/Viewport.h"

namespace Menge
{
	class Camera2D;
	class RenderViewport;

	class Layer2D
		: public Layer
	{
        DECLARE_VISITABLE()

	public:
		Layer2D();

	public:
		void setRenderViewport( const Viewport & _viewport );
		void removeRenderViewport();

    protected:
        void createRenderViewport_();
        void clearRenderViewport_();

	public:
		mt::vec2f cameraToLocal( Camera2D * _camera2D, const mt::vec2f& _point );				
		void calcScreenPosition( mt::vec2f & _screen, const RenderCameraInterface * _camera, Node* _node ) const override;

    protected:
        bool _activate() override;
        void _deactivate() override;
    
    protected:
		void _addChildren( Node * _node ) override;

	protected:
		Viewport m_viewport;
		
		Camera2D * m_camera2D;
		RenderViewport * m_renderViewport;
		//Viewport m_viewportWM;
		//mt::mat4f m_viewMatrix;
		//mt::mat4f m_projectionMatrix;

		bool m_hasViewport;
	};
}
