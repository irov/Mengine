#	pragma once

#	include "Viewport.h"

#	include "Math/mat3.h"

class TiXmlElement;

namespace Menge
{
	class RenderEngine;

	class Renderable
	{
	public:
		Renderable();

	public:
		virtual bool renderSelf( const mt::mat3f & _rwm, const Viewport & _viewPort );
		virtual void hide( bool value );
		virtual bool isVisible( const Viewport & _viewPort );

	protected:
		virtual void _render( const mt::mat3f &rwm, const Viewport & _viewPort );

	public:
		virtual const Viewport & updateViewport( const Viewport & _viewPort );			

	public:
		void loader( TiXmlElement * _xml );

	private:
		bool m_hide;
	};
}