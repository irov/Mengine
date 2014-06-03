#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class Isometric
		: public Node
	{
		DECLARE_VISITABLE()

	public:
		Isometric();

	protected:
		void render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask ) override;		
	};
}
