#	pragma once

#	include "Allocator2D.h"

#	include "Viewport.h"

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
		virtual bool render( const Viewport & _viewPort );
		virtual void hide(bool value);
		virtual bool isVisible( const Viewport & _viewPort);

	protected:
		virtual void _render( const mt::mat3f &rwm, const Viewport & _viewPort );

	public:
		virtual const Viewport & updateViewport( const Viewport & _viewPort );			

	private:
		bool m_hide;
		bool m_modifyViewPort;
	};
}