#	pragma once

#	include "Node.h"
#	include "Interface/PhysicSystem2DInterface.h"

#	include "Math/polygon.h"

class PhysicBody2DInterface;

namespace Menge
{
	//class PyObject;
	class RigidBody2D;

	class BodyListenerProxy
		: public PhysicBody2DListener
	{
	public:
		BodyListenerProxy( RigidBody2D* _body );

	public:
		void onCollide( PhysicBody2DInterface* _otherObj, float _worldX, float _worldY, float _normalX, float _normalY ) override;
		void applyForceAndTorque() override;

	private:
		RigidBody2D* m_body;
	};

	class RigidBody2D
		: public Node
		//, public PhysicBody2DCollisionListener
	{
		OBJECT_DECLARE( RigidBody2D )

	public:
		RigidBody2D();
		virtual ~RigidBody2D();

	public:
		void addShapeBox( float _width, float _heigth, const mt::vec2f& _pos, float _angle );

	public:
		virtual void onCollide( PhysicBody2DInterface* _otherObj, float _worldX, float _worldY, float _normalX, float _normalY );

	public:
		void loader( XmlElement * _xml ) override;

	public:
		void loaderPhysics_( XmlElement * _xml );

	protected:
		void loaderShape_( XmlElement * _xml );
		void loaderShapeCircle_( XmlElement * _xml );
		void loaderShapeBox_( XmlElement * _xml );

	// scripted
	public:
		void onApplyForceAndTorque();
		void applyForce( float _forceX, float _forceY, float _pointX, float _pointY );
		void applyImpulse( float _impulseX, float _impulseY, float _pointX, float _pointY );
		void applyConstantForce( float _forceX, float _forceY, float _pointX, float _pointY );
		void removeConstantForce();
		void setAngle( float _angle );
		void setPosition( float _x, float _y );
		void setDirection( const mt::vec2f& _dir );
		void setDirectionForce( bool _relative );
		float getMass() const;
		mt::vec2f getLinearVelocity() const;
		void setLinearVelocity( float _x, float _y, bool _countGravity );
		void wakeUp();

		PhysicBody2DInterface* getInterface();

	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

		void _setListener() override;
		void _debugRender( const Viewport & _viewport ) override;

	protected:
		BodyListenerProxy* m_listenerProxy;

		float m_linearDamping;
		float m_angularDamping;
		bool m_allowSleep;
		bool m_isBullet;
		bool m_fixedRotation;
		bool m_isSensor;
		float m_density;
		float m_friction;
		float m_restitution;
		unsigned int m_collisionMask;
		unsigned int m_categoryBits;
		unsigned int m_groupIndex;
		
		typedef std::vector<mt::polygon> TShapeList;
		TShapeList m_shapeList;
		
		typedef std::vector< std::pair< float, mt::vec2f > > TShapeCircleList;
		TShapeCircleList m_shapeCircleList;

		typedef std::vector< std::pair< std::pair< float, float >, std::pair< mt::vec2f, float > > > TShapeBoxList;
		TShapeBoxList m_shapeBoxList;

		mt::polygon m_shape;

	protected:
		PhysicBody2DInterface* m_interface;
		//PyObject* m_scriptListener;
		bool m_constantForce;
		bool m_directionForce;
		bool m_linearVelocity;
		mt::vec2f m_force;
		mt::vec2f m_forcePoint;
		mt::vec2f m_velocity;
		bool m_countGravity;
	};

}	// namespace Menge