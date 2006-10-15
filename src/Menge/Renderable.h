#	pragma once

#	include "Allocator.h"

#	include "math/mat3.h"

namespace Menge
{
	class RenderEngine;

	class Renderable
		: public Allocator
	{
		OBJECT_DECLARE(Renderable);

	public:
		virtual void render();
	};
}