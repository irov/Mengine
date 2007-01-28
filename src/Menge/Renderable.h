#	pragma once

#	include "Allocator2D.h"

#	include "math/mat3.h"

namespace Menge
{
	class RenderEngine;

	class Renderable
		: public Allocator2D
	{
		OBJECT_DECLARE(Renderable);
	public:
		Renderable();

	public:
		virtual void render();
		virtual void hide(bool value);

	protected:
		virtual void _render();

	private:
		bool m_hide;
	};
}