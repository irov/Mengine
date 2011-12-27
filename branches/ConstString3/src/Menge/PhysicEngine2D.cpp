#	include "PhysicEngine2D.h"
#	include	<algorithm>
#	include "LogEngine.h"
#	include "Interface/PhysicSystem2DInterface.h"

#	define MAX_BODIES 512

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	PhysicEngine2D::PhysicEngine2D()
		: m_interface(NULL)
		, m_timing(0.f)
		, m_timeStep(1.f/60.f)
		, m_iterating(10)
		, m_gravity(0.f, 0.f)
		, m_phase(0.f)
		, m_inProcess(false)
	{
		m_bodies.reserve( MAX_BODIES );
		//createScene( mt::vec2f( 0.0f, 0.0f ), mt::vec2f(2048.0f, 2048.0f), mt::vec2f(0.0f, 50.0f), true );
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicEngine2D::~PhysicEngine2D()
	{
		if( m_interface != NULL )
		{
			releaseInterfaceSystem( m_interface );
			m_interface = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool PhysicEngine2D::initialize()
	{
		bool result = initInterfaceSystem( &m_interface );
		if( ( result == false ) || ( m_interface == NULL ) )
		{
			return false;
		}

		
		return true;

	}
	//////////////////////////////////////////////////////////////////////////
	bool PhysicEngine2D::isWorldCreate() const
	{
		return m_inProcess;
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine2D::createWorld( const mt::vec2f& _upperLeft, const mt::vec2f& _lowerRight, const mt::vec2f& _gravity, bool _doSleep /* = true  */)
	{
		if( m_inProcess == false )
		{
			m_gravity = _gravity;
			m_interface->createWorld( _upperLeft, _lowerRight, m_gravity, _doSleep );
			m_inProcess = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine2D::destroyWorld()
	{
		m_interface->destroyWorld();
		m_inProcess = false;
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicBody2DInterface* PhysicEngine2D::createBody( const int bodyType, const mt::vec2f& _pos, float _angle, float _linearDamping, float _angularDamping,
															 bool _allowSleep, bool _isBullet, bool _fixedRotation )
	{
		PhysicBody2DInterface* body = m_interface->createBody( bodyType, _pos, _angle, _linearDamping, _angularDamping, _allowSleep, _isBullet, _fixedRotation );
		if( body )
		{
			m_bodies.push_back( body );
		}
		return body;
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine2D::destroyPhysicBody( PhysicBody2DInterface* _bodyInterface )
	{
		TVectorBodies::iterator it_find = std::find( m_bodies.begin(), m_bodies.end(), _bodyInterface );
		if( it_find != m_bodies.end() )
		{
			m_bodies.erase( it_find );
		}
		m_interface->destroyBody( _bodyInterface );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine2D::update( float _timing )
	{
		if( m_inProcess == false )
		{
			return;
		}
		m_timing += _timing * 0.001f;
		while( m_timing >= m_timeStep )
		{
			
			for( TVectorBodies::iterator it = m_bodies.begin(), it_end = m_bodies.end();
				it != it_end;
				it++ )
			{
				PhysicBody2DInterface* body = (*it);
				if( !(body->isFrozen() || body->isStatic() || body->isSleeping()) )
				{
					body->update();
				}
			}
			m_interface->update( m_timeStep, m_iterating, 8 );
			m_timing -= m_timeStep;
		}		
		//m_interface->update( m_timeStep, m_iterating, 8 );
		m_phase = m_timing / m_timeStep;
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicJoint2DInterface* PhysicEngine2D::createDistanceJoint( PhysicBody2DInterface* _body1, PhysicBody2DInterface* _body2, const mt::vec2f& _offsetBody1, const mt::vec2f& _offsetBody2, bool _collideBodies )
	{
		if( _body1 == 0 || _body2 == 0 )
		{
			MENGE_LOG_ERROR( "createDistanceJoint body1 - %p body 2 - %p"
				, _body1
				, _body2 
				);

			return 0;
		}

		return m_interface->createDistanceJoint( _body1, _body2, _offsetBody1, _offsetBody2, _collideBodies );
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicJoint2DInterface* PhysicEngine2D::createHingeJoint( PhysicBody2DInterface* _body1, PhysicBody2DInterface* _body2, const mt::vec2f& _offsetBody1, const mt::vec2f& _limits, bool _collideBodies )
	{
		return m_interface->createHingeJoint( _body1, _body2, _offsetBody1, _limits, _collideBodies );
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicJoint2DInterface * PhysicEngine2D::createPrismaticJoint(
		Menge::PhysicBody2DInterface* _body1
		, Menge::PhysicBody2DInterface* _body2
		, const mt::vec2f& _unitsWorldAxis
		, bool _collideConnected 
		, bool _enableLimit
		, const mt::vec2f& _translation 
		, bool _enableMotor
		, float _maxMotorForce
		, float _motorSpeed)
	{
		return m_interface->createPrismaticJoint( _body1, _body2, _unitsWorldAxis, _collideConnected ,_enableLimit, _translation, _enableMotor, _maxMotorForce, _motorSpeed );
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicJoint2DInterface * PhysicEngine2D::createPulleyJoint(
		Menge::PhysicBody2DInterface* _body1
		, Menge::PhysicBody2DInterface* _body2
		, const mt::vec2f& _offsetBody1
		, const mt::vec2f& _offsetBody2
		, const mt::vec2f& _offsetGroundBody1
		, const mt::vec2f& _offsetGroundBody2
		, float _ratio
		, bool _collideConnected)
	{
		return m_interface->createPulleyJoint( _body1, _body2, _offsetBody1, _offsetBody2, _offsetGroundBody1, _offsetGroundBody2, _ratio, _collideConnected );
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicJoint2DInterface * PhysicEngine2D::createGearJoint(
		Menge::PhysicBody2DInterface * _body1
		, Menge::PhysicBody2DInterface * _body2
		, Menge::PhysicJoint2DInterface * _joint1
		, Menge::PhysicJoint2DInterface * _joint2
		, float _ratio
		, bool _collideConnected)
	{
		return m_interface->createGearJoint( _body1, _body2, _joint1, _joint2, _ratio, _collideConnected );
	}	
	//////////////////////////////////////////////////////////////////////////
	PhysicJoint2DInterface * PhysicEngine2D::createRopeJoint( 
		Menge::PhysicBody2DInterface * _body1
		, Menge::PhysicBody2DInterface * _body2
		, const mt::vec2f & _localAnchor1
		, const mt::vec2f & _localAnchor2
		, float _maxlength
		, bool _collideConnected )
	{
		return m_interface->createRopeJoint( _body1, _body2, _localAnchor1, _localAnchor2, _maxlength ,_collideConnected );
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicJoint2DInterface * PhysicEngine2D::createWheelJoint( 
		Menge::PhysicBody2DInterface * _body1, Menge::PhysicBody2DInterface * _body2
		, const mt::vec2f & _localAnchor1
		, const mt::vec2f & _localAnchor2
		, const mt::vec2f & _localAxis1
		, float _frequencyHz
		, float _dampingRatio
		, bool _collideConnected
		, float _maxMotorTorque
		, float _motorSpeed	)
	{
		return m_interface->createWheelJoint( _body1, _body2, _localAnchor1, _localAnchor2, _localAxis1
											, _frequencyHz, _dampingRatio, _collideConnected, _maxMotorTorque, _motorSpeed );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine2D::destroyJoint( PhysicJoint2DInterface* _joint )
	{
		m_interface->destroyJoint( _joint );
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& PhysicEngine2D::getGravity() const
	{
		return m_gravity;
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicJoint2DInterface* PhysicEngine2D::createMouseJoint( PhysicBody2DInterface* _body, int _x, int _y  )
	{
		return m_interface->createMouseJoint( _body, _x, _y );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicEngine2D::onMouseMove( const mt::vec2f & _delta )
	{
		static float x;
		static float y;
		if( _delta.x == x && _delta.y == y)
			return;
		x = _delta.x;
		y = _delta.y;
		m_interface->onMouseMove( _delta.x, _delta.y ); 
	}
	//////////////////////////////////////////////////////////////////////////
	float PhysicEngine2D::getPhase() const
	{
		return m_phase;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge
