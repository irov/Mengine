#	pragma once

#	include "Allocator2D.h"
#	include "Viewport.h"

namespace Menge
{
	class CameraBehavior;
	class Scene;
	class Viewport;

	class Camera2D
		: public Allocator2D
	{
		OBJECT_DECLARE( Camera2D )
	public:
		Camera2D();

	public:
		const Viewport & getViewport() const;

	public:
		void setViewportSize( const mt::vec2f & _size );
		
	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _update( float _timing ) override;
		void _changePivot() override;

	protected:
		virtual void updateViewport();

	protected:
		Scene * m_renderScene;

		Viewport m_viewport;

		mt::vec2f m_viewportSize;
	};
}