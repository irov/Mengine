#	pragma once

#	include "Math/mat3.h"

class TiXmlElement;

namespace Menge
{
	class RenderEngine;
	class Viewport;

	class Renderable2D
	{
	public:
		Renderable2D();

	public:
		virtual void render( const mt::mat3f & _rwm, const Viewport & _viewport );
		virtual void hide( bool value );
		virtual bool isVisible( const Viewport & _viewport );

		virtual const Viewport & updateViewport( const Viewport & _viewport );

	protected:
		virtual void _render( const mt::mat3f & _rwm, const Viewport & _viewport );

	public:
		void loader( TiXmlElement * _xml );

	private:
		bool m_hide;
	};
}