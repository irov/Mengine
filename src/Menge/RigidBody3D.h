#	pragma once

#	include "NodeSinglethon.h"
#	include "math/quat.h"

#	include "MotionModifier.h"

class RigidBodyInterface;

namespace Menge
{
	class SceneNode3D;
	class ResourcePhysicGeometry;

	class RigidBody3D
		: public NodeSinglethon
	{
		OBJECT_DECLARE( RigidBody3D )

	public:
		RigidBody3D();
		~RigidBody3D();
	public:
		void applyImpulse( const mt::vec3f & _vec );
		void applyAngularImpulse( const mt::vec3f & _vec );
		void applyForce( const mt::vec3f & _vec );
		void applyTorque( const mt::vec3f & _vec );
		void setLinearVelocity( const mt::vec3f & _vec );
		void setAngularVelocity( const mt::vec3f & _vec );
		void setActive( bool _active );

		void setPosition( const mt::vec3f & _position );
		void setOrientation( const mt::quatf & _quat );

		const mt::vec3f & getPosition();
		const mt::quatf & getOrientation();

	public:
		void loader( XmlElement * _xml ) override;
	
	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

	private:
		bool m_active;
		float m_density;

		RigidBodyInterface * m_interface;

		std::string m_resourcename;
		ResourcePhysicGeometry * m_resource;

		void render(){}
		bool isRenderable(){return false;}
	};
}