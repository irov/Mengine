#	pragma once

#	include "Kernel/Node.h"

namespace Menge
{
	class Isometric
		: public Node
	{
		DECLARE_VISITABLE( Node )

	public:
		Isometric();

	protected:
		void render( RenderServiceInterface * _renderService, const RenderObjectState * _state, uint32_t _debugMask ) override;
	};
}
