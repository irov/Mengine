#	pragma once

#	include "Node.h"
#	include "InputHandler.h"

namespace Menge
{
	class Camera2D;

	class Layer
		: public virtual Node
		, public InputHandler
	{
	public:
		virtual void renderLayer() = 0;
	};
}