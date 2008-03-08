#	pragma once

#	include "SceneNode3D.h"

class CameraInterface;

namespace Menge
{
	class Camera3D
		: public SceneNode3D
	{
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
	
		void yaw( float _angle );
		void pitch( float _angle );
		void roll( float _angle );

	public:
		SceneNode3D * getParentNode() const;
		void setParentNode( SceneNode3D * _node );

	public:
		void loader( XmlElement * _xml );

		bool activate();
		void deactivate();
		void release();

		const std::string & getName();

	

	private:
		SceneNode3D * m_parent;

		CameraInterface * m_interface;
		
		float m_aspect;
		float m_near;
		float m_far;

		mt::vec3f m_position;
		mt::vec3f m_at;
		std::string m_name;
	};
}