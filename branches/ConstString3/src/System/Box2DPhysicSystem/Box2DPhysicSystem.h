#	pragma once

#	include "../../Interface/PhysicSystem2DInterface.h"
#	include "Box2D/Box2D.h"
#	include <vector>

class Box2DPhysicJoint;
class Box2DPhysicBody;
class Box2DPhysicScaler;

class Box2DPhysicSystem
	: public Menge::PhysicSystem2DInterface
	, public b2ContactListener
{
public:
	Box2DPhysicSystem();
	virtual ~Box2DPhysicSystem();

	void createWorld( const mt::vec2f& _upperLeft,  const mt::vec2f& _lowerRight,  const mt::vec2f& _gravity, bool _doSleep ) override;
	void destroyWorld() override;
	
	void update( float _timing, int _velocityIterations, int _positionIterations ) override;

	Menge::PhysicBody2DInterface* createBody( const int bodyType, const mt::vec2f& _pos, float _angle, float _linearDamping, float _angularDamping,
												bool _allowSleep, bool _isBullet, bool _fixedRotation ) override;

	void destroyBody( Menge::PhysicBody2DInterface* _body ) override;
	
	
	Menge::PhysicJoint2DInterface * createDistanceJoint( Menge::PhysicBody2DInterface* _body1,
														 Menge::PhysicBody2DInterface* _body2,
														 const mt::vec2f& _offsetBody1,
														 const mt::vec2f& _offsetBody2,
														 bool _collideBodies ) override;

	Menge::PhysicJoint2DInterface * createHingeJoint( Menge::PhysicBody2DInterface* _body1,
													  Menge::PhysicBody2DInterface* _body2,
													  const mt::vec2f& _offsetBody1,
													  const mt::vec2f& _limits,
													  bool _collideBodies ) override;
	
	Menge::PhysicJoint2DInterface * createMouseJoint( Menge::PhysicBody2DInterface* _body, int _x, int _y  ) override;

	Menge::PhysicJoint2DInterface * createPrismaticJoint( Menge::PhysicBody2DInterface* _body1
														, Menge::PhysicBody2DInterface* _body2
														, const mt::vec2f& _unitsWorldAxis
														, bool _collideConnected 
														, bool _enableLimit
														, const mt::vec2f& _translation 
														, bool _enableMotor
														, float _maxMotorForce
														, float _motorSpeed) override;

	Menge::PhysicJoint2DInterface * createPulleyJoint( Menge::PhysicBody2DInterface* _body1
													 , Menge::PhysicBody2DInterface* _body2
													 , const mt::vec2f& _offsetBody1
													 , const mt::vec2f& _offsetBody2
													 , const mt::vec2f& _offsetGroundBody1
													 , const mt::vec2f& _offsetGroundBody2
													 , float _ratio
													 , bool _collideConnected ) override;

	Menge::PhysicJoint2DInterface * createGearJoint	( Menge::PhysicBody2DInterface * _body1
													, Menge::PhysicBody2DInterface * _body2
													, Menge::PhysicJoint2DInterface * _joint1
													, Menge::PhysicJoint2DInterface * _joint2
													, float _ratio
													, bool _collideConnected ) override;	
	
	Menge::PhysicJoint2DInterface * createRopeJoint	( Menge::PhysicBody2DInterface * _body1
													, Menge::PhysicBody2DInterface * _body2
													, const mt::vec2f & _offsetBody1
													, const mt::vec2f & _offsetBody2
													, float _maxlength
													, bool _collideConnected ) override;	
	
	
	Menge::PhysicJoint2DInterface * createWheelJoint( Menge::PhysicBody2DInterface * _body1
													, Menge::PhysicBody2DInterface * _body2
													, const mt::vec2f & _localAnchor1
													, const mt::vec2f & _localAnchor2
													, const mt::vec2f & _localAxis1
													, float _frequencyHz
													, float _dampingRatio
													, bool _collideConnected 
													, float _maxMotorTorque
													, float _motorSpeed		
													) override;	
	
	void destroyJoint( Menge::PhysicJoint2DInterface* _joint );
	void onMouseMove( float x, float y ) override;
	
	void BeginContact(b2Contact* contact); 
	void EndContact(b2Contact* contact);
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse); 
private:
	struct FindContact
	{
		FindContact( const b2Contact* _contact )
			: m_contact( _contact )
		{
		}

		bool operator()( const b2Contact* _contact )
		{
			bool result =
				(m_contact->GetFixtureA() == _contact->GetFixtureA() ) 
				&& (m_contact->GetFixtureB() == _contact->GetFixtureB() );
			
			if(result == true)
			{
				int x =1;
			}

			return result;
		}

	private:
		const b2Contact* m_contact;
	};

private:
	 b2World* m_world;
	 b2Body*  m_ground;
	 typedef std::vector< b2Contact * > TVectorContact;
	 TVectorContact m_contacts;
	 Box2DPhysicScaler* m_scaler;

	 typedef std::vector< std::pair< b2JointDef*, Box2DPhysicJoint* > > TJointDefList;
	 TJointDefList m_jointDefList;

	 typedef std::vector< Box2DPhysicBody* > TBodyVector;
	 TBodyVector m_deletingBodies;

	void _createJoint( b2JointDef* _jointDef, Box2DPhysicJoint* _joint );
	b2MouseJoint* m_mouseJoint;
};
