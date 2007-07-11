#	pragma once

#	include "Node.h"
#	include "Renderable2D.h"

namespace Menge
{
	class Camera2D;

	class Layer
		: public virtual Node
		, public Renderable2D
	{
	public:

		virtual void renderLayer() = 0;
	};
}