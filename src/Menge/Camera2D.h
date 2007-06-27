#	pragma once

#	include "SceneNode2D.h"
#	include "Viewport.h"

namespace Menge
{
	class Scene;
	class Viewport;

	class Camera2D
		: public SceneNode2D
	{
		OBJECT_DECLARE( Camera2D )
	public:
		Camera2D();
		~Camera2D();

	public:
		const Viewport & getViewport() const;

	public:
		void setViewportSize( const mt::vec2f & _size );
		
	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		bool _activate() override;
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