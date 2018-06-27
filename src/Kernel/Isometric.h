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
        ~Isometric() override;

	protected:
		void render( const RenderContext * _state ) override;
	};
}
