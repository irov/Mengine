#	include "PhysicalBody2D.h"
#	include "Camera2D.h"

#	include "PhysicEngine2D.h"
#	include "RenderEngine.h"

#	include "Math/vec2.h"
#	include "Math/angle.h"

#	include "Application.h"

namespace Menge
{
	////////////////////////////////////////////////////////////////////////
	PhysicalBody2D::PhysicalBody2D()
		: m_interface( NULL )
		, m_density( 0.0f )
		, m_friction( 1.0f )
		, m_restitution( 0.0f )
		, m_force( 0.0f, 0.0f )
		, m_forcePoint( 0.0f, 0.0f )
		, m_constantForce( false )
		, m_directionForce( false )
		, m_linearDamping( 0.0f )
		, m_angularDamping( 0.0f )
		, m_allowSleep( true )
		, m_isBullet( false )
		, m_fixedRotation( false )
		, m_collisionMask( 0xFFFF )
		, m_categoryBits( 1 )
		, m_groupIndex( 0 )
		, m_isSensor( false )
		, m_linearVelocity( false )
		, m_countGravity( true )
		, m_unsetLinearVelocity( false )
		, m_stabilityAngle( 0.0f )
		, m_stabilization( false )
		, m_stabilityForce( 1.0f )
		, m_angle( 0.0f )
		, m_position( 0.0f , 0.0f)
		, m_limits( 1024.0f, 760.0f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicalBody2D::~PhysicalBody2D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::onCollide( PhysicBody2DInterface * _otherObj, float _worldX, float _worldY, float _normalX, float _normalY )
	{
		PhysicalBody2D * other = static_cast<PhysicalBody2D*>( _otherObj->getUserData() );
		this->callEvent( EVENT_COLLIDE, "(OOffff)", this->getEmbed(), other->getEmbed(), _worldX, _worldY, _normalX, _normalY );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::loader( BinParser * _parser )
	{
		Node::loader( _parser );
		/*
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE( Protocol::Density_Value, m_density );
			BIN_CASE_ATTRIBUTE( Protocol::Friction_Value, m_friction );
			BIN_CASE_ATTRIBUTE( Protocol::Restitution_Value, m_restitution );
			BIN_CASE_ATTRIBUTE( Protocol::CollisionMask_Value, m_collisionMask );
			BIN_CASE_ATTRIBUTE( Protocol::CategoryBits_Value, m_categoryBits );
			BIN_CASE_ATTRIBUTE( Protocol::GroupIndex_Value, m_groupIndex );
			BIN_CASE_ATTRIBUTE( Protocol::LinearDamping_Value, m_linearDamping );
			BIN_CASE_ATTRIBUTE( Protocol::AngularDamping_Value, m_angularDamping );
			BIN_CASE_ATTRIBUTE( Protocol::AllowSleep_Value, m_allowSleep );
			BIN_CASE_ATTRIBUTE( Protocol::IsBullet_Value, m_isBullet );
			BIN_CASE_ATTRIBUTE( Protocol::FixedRotation_Value, m_fixedRotation );
			BIN_CASE_ATTRIBUTE( Protocol::IsSensor_Value, m_isSensor );
		}
		*/
	}
	
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::_update( float _timing )
	{
		if(m_interface == NULL)
		{
			return;
		}
		bool isSleeping = m_interface->isSleeping();
		bool isFrozen = m_interface->isFrozen();
		bool isStatic = m_interface->isStatic();
		if( !(isSleeping || isStatic || isFrozen) )
		{
			const float phase = PhysicEngine2D::get()->getPhase();

			const mt::vec2f& pos = m_interface->getPosition();
			const mt::vec2f& prevPos = getLocalPosition();
			mt::vec2f currPos( pos[0], m_limits.y - pos[1] );
			//currPos = currPos * phase + prevPos * ( 1.0f - phase );
			setLocalPosition( currPos );

			const mt::vec2f& orient = m_interface->getOrientation();
			setLocalDirection( mt::vec2f( orient[0], orient[1] ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool PhysicalBody2D::_compile()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::_release()
	{
		PhysicEngine2D::get()->destroyPhysicBody( m_interface );
		m_interface = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::applyForce( float _forceX, float _forceY, float _pointX, float _pointY )
	{
		const mt::vec2f & position = getLocalPosition();

		m_interface->applyForce( _forceX, _forceY, position.x + _pointX, position.y + _pointY );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::applyImpulse( float _impulseX, float _impulseY, float _pointX, float _pointY )
	{
		const mt::vec2f & position = getLocalPosition();

		m_interface->applyImpulse( _impulseX, _impulseY, position.x + _pointX, position.y + _pointY );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::applyConstantForce( float _forceX, float _forceY, float _pointX, float _pointY )
	{
		m_force.x = _forceX;
		m_force.y = _forceY;
		m_forcePoint.x = _pointX;
		m_forcePoint.y = _pointY;
		m_constantForce = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::removeConstantForce()
	{
		m_constantForce = false;
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicBody2DInterface * PhysicalBody2D::getInterface()
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setDirectionForce( bool _relative )
	{
		m_directionForce = _relative;
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::wakeUp()
	{
		if( m_interface )
		{
			m_interface->wakeUp();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float PhysicalBody2D::getMass() const
	{
		return m_interface->getMass();
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f PhysicalBody2D::getLinearVelocity() const
	{
		const mt::vec2f& v = m_interface->getLinearVelocity();
		return mt::vec2f( v[0], v[1] );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setLinearVelocity( float _x, float _y, bool _countGravity )
	{
		m_constantForce = false;
		m_linearVelocity = true;
		m_velocity.x = _x;
		m_velocity.y = _y;
		m_countGravity = _countGravity;

		if( m_interface )
		{
			m_interface->wakeUp();
		}
		//m_interface->setLinearVelocity( _x, _y );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::addShapeConvex( int _pointsNum, const Polygon & _polygon )
	{
		float * _convex = NULL;

		m_interface->addShapeConvex( _pointsNum, _convex, m_density, m_friction
				, m_restitution, m_isSensor, m_collisionMask, m_categoryBits, m_groupIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::addShapeCircle( float _radius, mt::vec2f _localPos )
	{
		m_interface->addShapeCircle(  _radius, _localPos, m_density, m_friction , m_restitution, m_isSensor, m_collisionMask, m_categoryBits, m_groupIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::addShapeBox( float _width, float _height, mt::vec2f _localPos, float _angle )
	{
		m_interface->addShapeBox( _width, _height ,_localPos , _angle ,m_density, m_friction , m_restitution, m_isSensor, m_collisionMask, m_categoryBits, m_groupIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	void  PhysicalBody2D::createBody( int _type/*, float _angle, float _linearDamping, float _angularDamping, float _allowSleep, bool _isBullet, float _fixedRotation*/ )
	{
		const mt::vec2f & position = getLocalPosition();

		m_interface = PhysicEngine2D::get()->createBody( _type, position, 0.0f, m_linearDamping, m_angularDamping, m_allowSleep,
			m_isBullet, m_fixedRotation );

		if( m_interface == 0 )
		{
			MENGE_LOG_ERROR( "PhysicalBody2D: Error while compiling. can't create Body"
				);
			
			return;
		}
		
		m_interface->setBodyListener( this );
		m_interface->setUserData( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::_setEventListener( PyObject * _listener )
	{
		Node::_setEventListener( _listener );
		
		Eventable::registerEvent( EVENT_COLLIDE, ("onCollide"), _listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::onApplyForceAndTorque_()
	{
		if( m_constantForce )
		{
			//const mt::vec2f & position = getWorldPosition();
			mt::vec2f force;
			mt::vec2f point;

			if( m_directionForce )
			{
				mt::mul_v2_m3_r( force, m_force, getWorldMatrix() );
			}
			else
			{
				force = m_force;
			}

			//force *= /*m_interface->getMass() **/ _timing;

			mt::mul_v2_m3( point, m_forcePoint, getWorldMatrix() );

			m_interface->applyForce( force.x, force.y, point.x, point.y );
		}
		else if( m_linearVelocity )
		{
			const mt::vec2f& v = m_interface->getLinearVelocity();
			mt::vec2f cv( v[0], v[1] );
			mt::vec2f force = ( m_velocity - cv ) * m_interface->getMass() * 60.0f;
			if( m_countGravity )
			{
				force -= PhysicEngine2D::get()->getGravity() * m_interface->getMass();
			}
			const mt::vec2f& pos = m_interface->getPosition();
			m_interface->applyForce( force.x, force.y, pos[0], pos[1] );
			if( m_unsetLinearVelocity )
			{
				m_linearVelocity = false;
				m_unsetLinearVelocity = false;
			}
		}
		if( m_stabilization )
		{
			/*float dif = m_interface->getAngle() - m_stabilityAngle;
			if( fabsf( dif ) > 0.1f )
			{
			m_interface->applyTorque( -dif * fabsf( dif ) * m_stabilityForce );
			}*/
			float f = ( m_stabilityAngle - m_interface->getAngle() ) * 10.0f;//  * 0.0005f;
			float angvel = m_interface->getAngularVelocity();
			float torque = ( f - m_interface->getAngularVelocity() ) * m_interface->getInertia() * 1.0f;
			m_interface->applyTorque( torque );
		}

	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::_render( Camera2D * _camera )
	{
		Node::_render( _camera );
	}
#	ifndef MENGE_MASTER_RELEASE
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::_debugRender( Camera2D * _camera, unsigned int _debugMask )
	{
		if( _debugMask & MENGE_DEBUG_PHYSICS )
		{
			if(m_interface != NULL)
			{
				m_interface->dump();
			}
		}
	}
#	endif
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::unsetLinearVelocity()
	{
		if( m_linearVelocity )
		{
			m_unsetLinearVelocity = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::_freeze( bool _value )
	{
		if( _value == true )	// completly remove from simulation
		{
			_release();
		}
		else
		{
			_compile();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setFilterData(  int _categoryBits ,int _mask , int _groupIndex )
	{
		m_categoryBits = _categoryBits;
		m_collisionMask = _mask;
		m_groupIndex = _groupIndex;
		updateFilterData_();
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::updateFilterData_()
	{
		m_interface->updateFilterData( m_categoryBits, m_collisionMask, m_groupIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::onUpdate()
	{
		onApplyForceAndTorque_();

		const mt::vec2f& pos = m_interface->getPosition();
		mt::vec2f currPos( pos[0], pos[1] );
		setLocalPosition( currPos );

		const mt::vec2f& orient = m_interface->getOrientation();
		setLocalDirection( mt::vec2f( orient[0], orient[1] ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::compileShapes_()
	{
		/*for( int i = 0; i < m_shapeList.size(); )
		{
			if( m_shapeList[i].num_points() == 4 )
			{
				const Menge::TVectorPoints& pts = m_shapeList[i].get_points();
				if( pts[0].x == pts[3].x &&
					pts[0].y == pts[1].y &&
					pts[1].x == pts[2].x &&
					pts[2].y == pts[3].y )
				{
					float w = pts[1].x - pts[0].x;
					float h = pts[2].y - pts[1].y;
					mt::vec2f c( w * 0.5f, h * 0.5f );
					//mt::vec2f c( 0.0f, 0.0f );
					m_shapeBoxList.push_back( std::make_pair( std::make_pair( w, h ), std::make_pair( c, 0.0f ) ) );
					m_shapeList.erase( std::remove( m_shapeList.begin(), m_shapeList.end(), m_shapeList[i] ) );
					continue;
				}
			}
			i++;
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		Node::_updateBoundingBox( _boundingBox );
		/*
		const mt::mat3f& wm = getWorldMatrix();
		for( TShapeList::iterator it = m_shapeList.begin(),
			it_end = m_shapeList.end();
			it != it_end;
		it++ )
		{
			for( std::size_t 
				it_point = 0,
				it_point_end = (*it).num_points(); 
			it_point != it_point_end;
			++it_point )
			{
				mt::vec2f p;
				mt::mul_v2_m3( p, (*it)[it_point], wm );
				mt::add_internal_point( _boundingBox, p );
			}
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::enableStabilization( bool _enable, float _stabilityAngle, float _stabilityForce )
	{
		m_stabilization = _enable;
		m_stabilityAngle = _stabilityAngle;
		m_stabilityForce = _stabilityForce;
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setDensity( float _density )
	{
		m_density = _density;	
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setFriction( float _friction )
	{
		m_friction = _friction;
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setRestitution( float _restitution )
	{
		m_restitution = _restitution;
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setCollisionMask( int _mask )
	{
		m_collisionMask = (uint16)_mask;
		updateFilterData_();
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setCategoryBits( int _categoryBits )
	{
		m_categoryBits = _categoryBits;
		updateFilterData_();
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setGroupIndex( int _groupIndex )
	{
		m_groupIndex = _groupIndex;
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setLinearDumping( float _linearDamping )
	{
		m_linearDamping = _linearDamping;
		
		if( m_interface == NULL)
		{
			return;
		}
		
		m_interface->setLinearDumping( _linearDamping );
		
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setAngularDumping( float _angularDamping )
	{
		m_angularDamping = _angularDamping;
		
		if( m_interface == NULL)
		{
			return;
		}
		
		m_interface->setAngularDumping( _angularDamping );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setAllowSleep( bool _allowSleep )
	{
		m_allowSleep = _allowSleep;
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setIsBullet( bool _isBullet )
	{
		m_isBullet = _isBullet;

		if( m_interface == NULL)
		{
			return;
		}

		m_interface->setIsBullet( _isBullet );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setFixedRotation( bool _fixedRotation )
	{
		m_fixedRotation = _fixedRotation;

		if( m_interface == NULL)
		{
			return;
		}

		m_interface->setFixedRotation( _fixedRotation );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setIsSensor( bool isSensor )
	{
		m_isSensor = isSensor;
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setOrientation( float _angle )
	{
		if( m_interface == NULL)
		{
			return;
		}
		m_interface->setOrientation( _angle );
	}
	//////////////////////////////////////////////////////////////////////////
	void PhysicalBody2D::setLocalPosition( const mt::vec2f & _position )
	{
		Node::setLocalPosition( _position );
		m_interface->setPosition( _position.x, _position.y );
	}
	//////////////////////////////////////////////////////////////////////////
	float PhysicalBody2D::getDensity()
	{
		return m_density;
	}
	//////////////////////////////////////////////////////////////////////////
	float PhysicalBody2D::getFriction()
	{
		return m_friction;
	}
	//////////////////////////////////////////////////////////////////////////
	float PhysicalBody2D::getRestitution()
	{
		return m_restitution;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t PhysicalBody2D::getCollisionMask()
	{
		return m_collisionMask;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t PhysicalBody2D::getCategoryBits()
	{
		return m_categoryBits;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t PhysicalBody2D::getGroupIndex()
	{
		return m_groupIndex;
	}
	//////////////////////////////////////////////////////////////////////////
	float PhysicalBody2D::getLinearDumping()
	{
		return m_linearDamping;
	}
	//////////////////////////////////////////////////////////////////////////
	float PhysicalBody2D::getAngularDumping()
	{
		return m_angularDamping;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PhysicalBody2D::getAllowSleep()
	{
		return m_allowSleep;
	}
	//////////////////////////////////////////////////////////////////////////
	bool PhysicalBody2D::getIsBullet()
	{
		return m_isBullet;
	}
	//////////////////////////////////////////////////////////////////////////
	float PhysicalBody2D::getFixedRotation()
	{
		return m_fixedRotation;
	}
	//////////////////////////////////////////////////////////////////////////
	float PhysicalBody2D::getIsSensor()
	{
		return m_isSensor;
	}
	//////////////////////////////////////////////////////////////////////////
}

		
	
	

	


		
		
	