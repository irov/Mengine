#	pragma once

#	include "Math/mat4.h"

class TiXmlElement;

namespace Menge
{
	class Camera3D;

	class Renderable3D
	{
	public:
		Renderable3D();

	public:
		virtual void render( const mt::mat4f & _rwm, const Camera3D * _camera );
		virtual void hide( bool value );
		virtual bool isVisible( const Camera3D * _camera );

	protected:
		virtual void _render( const mt::mat4f & _rwm, const Camera3D * _camera );

	public:
		void loader( TiXmlElement * _xml );

	private:
		bool m_hide;
	};
}