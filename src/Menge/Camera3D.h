#	pragma once

#	include "SceneNode3D.h"

class CameraInterface;

namespace Menge
{
	class Camera3D
		: public SceneNode3D
	{
		OBJECT_DECLARE( Camera3D )

	public:
		Camera3D();
		~Camera3D();

	public:
		void lookAt(const mt::vec3f& _targetPoint);
		void setPosition(const mt::vec3f& _pos);
		void setNear( float _dist );
		void setFar( float _dist );
		void setAspect( float _aspect );
	//	mt::vec3f getDirectionFromMouse( float _xm, float _ym );

		void yaw( float _angle );
		void pitch( float _angle );
		void roll( float _angle );

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _activate() override;

	private:
		CameraInterface * m_interface;
		
		float m_aspect;
		float m_near;
		float m_far;
		mt::vec3f m_position;
		mt::vec3f m_at;
		bool m_main;

		void render(){}
		bool isRenderable(){return false;}
	};
}