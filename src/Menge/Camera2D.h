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

		void setTarget( SceneNode2D* _target );
		void enableTargetFollowing( bool _enable, float _force );
		
	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _activate() override;
		void _update( float _timing ) override;
		void _changePivot() override;

	protected:
		virtual void updateViewport();

	protected:
		Viewport m_viewport;

		mt::vec2f m_viewportSize;

		SceneNode2D* m_target;
		bool m_targetFollowing;
		float m_followingForce;
	};
}