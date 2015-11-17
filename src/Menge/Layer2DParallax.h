#	pragma once

#	include "Kernel/Layer.h"

#	include "RenderCamera.h"

namespace Menge
{
	class Layer2DParallax
		: public Layer
	{
        DECLARE_VISITABLE()

	public:
		Layer2DParallax();

	public:
		void setParallaxFactor( const mt::vec2f & _parallax );
		const mt::vec2f & getParallaxFactor() const;

	protected:
		void createRenderViewport_();
		void removeRenderViewport_();

	public:
		void calcScreenPosition( mt::vec2f & _screen, const RenderCameraInterface * _camera, Node* _node ) const override;

	protected:
		void render( const RenderObjectState * _state, unsigned int _debugMask ) override;

	protected:
		mt::vec2f m_parallax;

		RenderCamera m_camera[4];
	};
}
