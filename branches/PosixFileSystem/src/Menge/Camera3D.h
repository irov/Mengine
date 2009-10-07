#	pragma once

#	include "Camera.h"

#	include "SceneNode3D.h"

#	include "Frustum.h"

namespace Menge
{
	class Camera3D
		: public Camera
		, public SceneNode3D
		, public Frustum
	{
		FACTORABLE_DECLARE( Camera3D )
	public:
		Camera3D();

	public:
		const mt::mat4f & getViewMatrix() override;
		const mt::mat4f& getProjectionMatrix() override;
		const mt::vec4f& getRenderArea() override;
		bool is3D() const override;

	public:
		void lookAt(const mt::vec3f& _targetPoint);
		mt::vec3f getDirectionFromMouse( float _xm, float _ym );


		void loader( XmlElement * _xml ) override;

		void _render( unsigned int _debugMask ) override;
	protected:
		void _updateMatrix3D() override;
		

		bool _activate() override;

		bool _compile() override;
		void _release() override;

		mt::mat4f m_viewMatrix;	
		mt::vec3f m_at;

		mt::vec4f m_renderArea;
	};
}
