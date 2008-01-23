#	pragma once

#	include "SceneNode3D.h"

class Camera3dInterface;

namespace Menge
{
	class Camera3D
		: public SceneNode3D
	{
		OBJECT_DECLARE( Camera3D )

	public:
		Camera3D();

	public:
		void lookAt(const mt::vec3f& _targetPoint);
		void setPosition(const mt::vec3f& _pos);
		void setNear( float _dist );
		void setFar( float _dist );
		void setAspect( float _aspect );
	//	mt::vec3f getDirectionFromMouse( float _xm, float _ym );

	public:
		void loader( XmlElement * _xml ) override;

	private:
		Camera3dInterface * m_interface;
	};
}