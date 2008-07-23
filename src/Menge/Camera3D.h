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

		void lookAt(const mt::vec3f& _targetPoint);
		mt::vec3f getDirectionFromMouse( float _xm, float _ym );

		const mt::mat4f & getViewMatrix();

		void loader( XmlElement * _xml ) override;

		void _render( const Viewport & _viewport, bool _enableDebug ) override;
	protected:
		void _updateMatrix3D() override;
		

		bool _activate() override;


		mt::mat4f m_viewMatrix;	
		mt::vec3f m_at;
	};
}