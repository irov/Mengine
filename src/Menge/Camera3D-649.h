#	pragma once

#	include "SceneNode3D-649.h"

#	include "Frustum.h"

namespace Menge
{
	class Camera3D_
		: public SceneNode3D_
		, public Frustum
	{
		OBJECT_DECLARE( Camera3D_ )

	public:
		Camera3D_();

	public:
		void yaw( float _degrees );
		void pitch( float _degrees );
		
	public:
		void lookAt(const mt::vec3f& _targetPoint);
		mt::vec3f getDirectionFromMouse( float _xm, float _ym );

	public:
		const mt::mat4f & getViewMatrix();

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		void _updateMatrix( Allocator3D_ * _parent ) override;
		bool _renderBegin() override;

	protected:
		mt::mat4f m_viewMatrix;	
	};
}