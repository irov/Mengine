#	pragma once

#	include "SceneNode3D.h"

#	include "Frustum.h"

namespace Menge
{
	class Camera3D
		: public SceneNode3D
		, public Frustum
	{
		OBJECT_DECLARE( Camera3D )

	public:
		Camera3D();

	public:
		void yaw( float _degrees );
		void pitch( float _degrees );
		
	public:
		void lookAt(const mt::vec3f& _targetPoint);

		const mt::mat4f & getProjectionMatrix();
		const mt::mat4f & getViewMatrix();

		mt::vec3f getDirectionFromMouse( float _xm, float _ym );

	private:
		void recalcView();
		void recalcProjection();
		void recalcFrustum();

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		void _updateMatrix( Allocator3D * _parent ) override;

	protected:
		mt::mat4f m_viewMatrix;	
		mt::mat4f m_projMatrix;

		Frustum m_frustum;
	};
}