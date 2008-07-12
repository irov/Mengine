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
		BodyListenerProxy( RigidBody2D* _body )
			: m_body( _body )
		{
		}
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

		virtual void onCollide( PhysicBody2DInterface* _otherObj, float _worldX, float _worldY, float _normalX, float _normalY );

	public:
		virtual void loader( XmlElement * _xml ) override;

		void _loaderPhysics( XmlElement * _xml );
		void _addShapeBox( float _width, float _heigth, const mt::vec2f& _pos, float _angle );

		void _addShapePolygon( std::vector<mt::vec2f> & _points );

	// scripted
	public:
		PyObject* getListener();

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

		void _loaderShape( XmlElement * _xml );
		void _loaderShapeCircle( XmlElement * _xml );
		void _loaderShapeBox( XmlElement * _xml );

		BodyListenerProxy * m_listenerProxy;

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
		int m_groupIndex;
		
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
		//virtual void _onSetListener();

		void _render( const Viewport & _viewport, bool _enableDebug ) override;

	};

}	// namespace Menge