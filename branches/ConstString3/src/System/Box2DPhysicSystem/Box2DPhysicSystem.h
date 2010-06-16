#	pragma once

#	include "../../Interface/PhysicSystem2DInterface.h"

#	include "Box2D.h"
#	include <vector>

class Box2DPhysicJoint;
class Box2DPhysicBody;

class Box2DPhysicSystem
	: public Menge::PhysicSystem2DInterface
	, public b2ContactListener
{
public:
	Box2DPhysicSystem();
	virtual ~Box2DPhysicSystem();

	void createWorld( const float* _upperLeft, const float* _lowerRight, const float* _gravity, bool _doSleep ) override;
	void destroyWorld() override;
	void update( float _timing, int _velocityIterations, int _positionIterations ) override;

	Menge::PhysicBody2DInterface* createBody( const float* _pos, float _angle, float _linearDamping, float _angularDamping,
												bool _allowSleep, bool _isBullet, bool _fixedRotation ) override;

	void destroyBody( Menge::PhysicBody2DInterface* _body ) override;

	Menge::PhysicJoint2DInterface* createDistanceJoint( Menge::PhysicBody2DInterface* _body1, Menge::PhysicBody2DInterface* _body2, const float* _offsetBody1, const float* _offsetBody2, bool _collideBodies ) override;
	Menge::PhysicJoint2DInterface* createHingeJoint( Menge::PhysicBody2DInterface* _body1, Menge::PhysicBody2DInterface* _body2, const float* _offsetBody1, const float* _limits, bool _collideBodies ) override;
	Menge::PhysicJoint2DInterface* createMouseJoint( Menge::PhysicBody2DInterface* _body, int _x, int _y  ) override;
	void destroyJoint( Menge::PhysicJoint2DInterface* _joint );

	void onMouseMove( float x, float y ) override;

	void Add( const b2ContactPoint* point ) override;
	void Persist( const b2ContactPoint* point ) override;
	void Remove( const b2ContactPoint* point ) override;

private:
	struct FindContact
	{
		FindContact( const b2ContactPoint* _contact )
			: m_contact( _contact )
		{
		}

		bool operator()( const b2ContactPoint& _contact )
		{
			return (m_contact->shape1 == _contact.shape1)
				&& (m_contact->shape2 == _contact.shape2)
				&& (m_contact->id.key == _contact.id.key);
		}

	private:
		const b2ContactPoint* m_contact;
	};

private:
	 b2World* m_world;

	 typedef std::vector<b2ContactPoint> TVectorContactPoint;
	 TVectorContactPoint m_contacts;

	 typedef std::vector< std::pair< b2JointDef*, Box2DPhysicJoint* > > TJointDefList;
	 TJointDefList m_jointDefList;

	 typedef std::vector< Box2DPhysicBody* > TBodyVector;
	 TBodyVector m_deletingBodies;

	void _createJoint( b2JointDef* _jointDef, Box2DPhysicJoint* _joint );
	b2MouseJoint* m_mouseJoint;
};
