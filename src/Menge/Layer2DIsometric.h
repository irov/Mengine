#	pragma once

#	include "Layer2D.h"

#	include "RenderCamera.h"

namespace Menge
{
	class Layer2DIsometric
		: public Layer2D
	{
        DECLARE_VISITABLE()

	public:
		Layer2DIsometric();

	protected:
		void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, const RenderClipplaneInterface * _clipplane, unsigned int _debugMask ) override;
	};
}
