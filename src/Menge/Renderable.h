#	pragma once

#	include "Allocator2D.h"
#	include "ViewPort.h"

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
		virtual bool render( const ViewPort & _viewPort );
		virtual void hide(bool value);

	protected:
		virtual void _render( const ViewPort & _viewPort );

	public:
		virtual const ViewPort & updateViewPort( const ViewPort & _viewPort );			

	private:
		bool m_hide;
		bool m_modifyViewPort;
	};
}