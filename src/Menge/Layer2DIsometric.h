#	pragma once

#	include "Kernel/Layer.h"

#	include "RenderCamera.h"

namespace Menge
{
	class Layer2DIsometric
		: public Layer
	{
        DECLARE_VISITABLE()

	public:
		Layer2DIsometric();

	public:
		void calcScreenPosition( mt::vec2f & _screen, const RenderCameraInterface * _camera, Node* _node ) const override;
		    
	protected:
		void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask ) override;		
	};
}
