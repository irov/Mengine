#	pragma once

#	include "Math/mat4.h"

class TiXmlElement;

namespace Menge
{
	class RenderEngine;
	class Frustum;

	class Renderable3D
	{
	public:
		Renderable3D();

	public:
		virtual void render( const mt::mat4f & _rwm, const Frustum & _frustum );
		virtual void hide( bool value );
		virtual bool isVisible( const Frustum & _frustum );

		virtual const Frustum & updateFrustum( const Frustum & _frustum );

	protected:
		virtual void _render( const mt::mat4f & _rwm, const Frustum & _frustum );

	public:
		void loader( TiXmlElement * _xml );

	private:
		bool m_hide;
	};
}