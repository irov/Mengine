#	pragma once

#	include "SceneNode3D.h"

class CameraInterface;

namespace Menge
{
	class Camera3D
		: public SceneNode3D
	{
		OBJECT_DECLARE(Camera3D);
	public:
		Camera3D();
		Camera3D( const std::string & _name );
		~Camera3D();

	public:
		void lookAt(const mt::vec3f& _targetPoint);
		void setPosition(const mt::vec3f& _pos);
		void setNear( float _dist );
		void setFar( float _dist );
		void setAspect( float _aspect );
		mt::vec3f getDirection();
	
		virtual const mt::quatf & getLocalOrient();

		void yaw( float _angle );
		void pitch( float _angle );
		void roll( float _angle );

	public:
		void loader( XmlElement * _xml );

	protected:
		bool	_activate() override;
		void	_deactivate() override;
		bool	_compile() override;
		void	_release() override;
		//void	_update( float _timing ) override;

	protected:
		CameraInterface * m_camera;

	private:
		float m_near;
		float m_far;
		float m_aspect;

		mt::vec3f m_position;
		mt::vec3f m_at;
	};
}