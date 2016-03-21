#	pragma once

#	include "Kernel/Layer.h"

#	include "Core/Viewport.h"

namespace Menge
{
	class RenderCameraOrthogonal;
	class RenderViewport;

	class Layer2D
		: public Layer
	{
		DECLARE_VISITABLE( Layer )

	public:
		Layer2D();

	public:
		void setViewport( const Viewport & _viewport );
		void removeViewport();

    protected:
        void createViewport_();
        void clearViewport_();

	public:
		mt::vec2f cameraToLocal( const RenderCameraInterface * _camera2D, const mt::vec2f& _point );				
		void calcScreenPosition( mt::vec2f & _screen, const RenderCameraInterface * _camera, Node* _node ) const override;

    protected:
        bool _activate() override;
        void _deactivate() override;

	protected:
		Viewport m_viewport;
		
		RenderCameraOrthogonal * m_renderCamera;
		RenderViewport * m_renderViewport;

		bool m_hasViewport;
	};
}
