#	pragma once

#	include "Layer2D.h"

#	include "Kernel/RenderCamera.h"

namespace Menge
{
	class Layer2DIsometric
		: public Layer2D
	{
		DECLARE_VISITABLE( Layer2D )

	public:
		Layer2DIsometric();

	protected:
		void render( const RenderObjectState * _state, unsigned int _debugMask ) override;
	};
}
