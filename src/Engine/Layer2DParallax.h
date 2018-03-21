#	pragma once

#	include "Kernel/Layer.h"

#	include "Kernel/RenderCameraProxy.h"

namespace Menge
{
	class Layer2DParallax
		: public Layer
	{
		DECLARE_VISITABLE( Layer )

	public:
		Layer2DParallax();

	public:
		void setParallaxFactor( const mt::vec2f & _parallax );
		const mt::vec2f & getParallaxFactor() const;

	public:
		void setParallaxLoop( bool _loop );
		bool getParallaxLoop() const;

	protected:
		void createRenderViewport_();
		void removeRenderViewport_();

	public:
		void calcScreenPosition( mt::vec2f & _screen, const RenderCameraInterface * _camera, Node* _node ) const override;

	protected:
		void render( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state, unsigned int _debugMask ) override;

	protected:
		mt::vec2f m_parallax;

		RenderCameraProxy m_cameraNorm;
		RenderCameraProxy m_cameraLoop[4];

		bool m_parallaxLoop;
	};
}
