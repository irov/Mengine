#pragma once

#include "Kernel/Node.h"

namespace Mengine
{
	class Isometric
		: public Node
	{
		DECLARE_VISITABLE( Node )

	public:
		Isometric();

	protected:
		void render( RenderServiceInterface * _renderService, const RenderState * _state, uint32_t _debugMask ) override;
	};
}
