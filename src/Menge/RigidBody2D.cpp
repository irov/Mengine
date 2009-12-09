#	include "ObjectImplement.h"

#	include "RigidBody2D.h"
#	include "Camera2D.h"

#	include "PhysicEngine2D.h"
#	include "RenderEngine.h"
#	include "XmlEngine.h"

#	include "Math/vec2.h"
#	include "Math/angle.h"

#	include "Application.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void BodyListenerProxy::onCollide(PhysicBody2DInterface *_otherObj, float _worldX, float _worldY, float _normalX, float _normalY)
	{
		m_body->onCollide( _otherObj, _worldX, _worldY, _normalX, _normalY );
	}
	//////////////////////////////////////////////////////////////////////////
	void BodyListenerProxy::onUpdate()
	{
		m_body->onUpdate();
	}
	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT(RigidBody2D)
	//////////////////////////////////////////////////////////////////////////
	RigidBody2D::RigidBody2D()
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
	, m_bodyListener( 0 )
	, m_isSensor( false )
	, m_linearVelocity( false )
	, m_countGravity( true )
	, m_unsetLinearVelocity( false )
	, m_frozen( false )
	, m_stabilityAngle( 0.0f )
	, m_stabilization( false )
	, m_stabilityForce( 1.0f )

	{
	}
	//////////////////////////////////////////////////////////////////////////
	RigidBody2D::~RigidBody2D()
	{
		if( m_bodyListener )
		{
			delete m_bodyListener;
			m_bodyListener = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::onCollide( PhysicBody2DInterface * _otherObj, float _worldX, float _worldY, float _normalX, float _normalY )
	{
		RigidBody2D * other = static_cast<RigidBody2D*>( _otherObj->getUserData() );
		callEvent( EVENT_COLLIDE, "(OOffff)", this->getEmbedding(), other->getEmbedding(), _worldX, _worldY, _normalX, _normalY );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::loader( XmlElement * _xml )
	{
		Node::loader( _xml );
		_loaderPhysics( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_loaderPhysics( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Shape" )
			{
			/*	XML_FOR_EACH_ATTRIBUTES()
				{					
					XML_CASE_ATTRIBUTE( MENGE_TEXT("IsSensor"), filename );
				}*/

				mt::polygon n;
				m_shapeList.push_back( n );
				XML_PARSE_ELEMENT( this, &RigidBody2D::loaderShape_ );

			}
			XML_CASE_NODE( "ShapeCircle" )
			{
				std::pair< float, mt::vec2f > n;
				m_shapeCircleList.push_back( n );
				XML_PARSE_ELEMENT( this, &RigidBody2D::loaderShapeCircle_ );
			}
			XML_CASE_NODE( "ShapeBox" )
			{
				std::pair< std::pair< float, float >, std::pair< mt::vec2f, float > > n;
				m_shapeBoxList.push_back( n );
				XML_PARSE_ELEMENT( this, &RigidBody2D::loaderShapeBox_ );
			}

			XML_CASE_ATTRIBUTE_NODE( "Density", "Value", m_density );
			XML_CASE_ATTRIBUTE_NODE( "Friction", "Value", m_friction );
			XML_CASE_ATTRIBUTE_NODE( "Restitution", "Value", m_restitution );
			XML_CASE_ATTRIBUTE_NODE( "CollisionMask", "Value", m_collisionMask );
			XML_CASE_ATTRIBUTE_NODE( "CategoryBits", "Value", m_categoryBits );
			XML_CASE_ATTRIBUTE_NODE( "GroupIndex", "Value", m_groupIndex );
			XML_CASE_ATTRIBUTE_NODE( "LinearDamping", "Value", m_linearDamping );
			XML_CASE_ATTRIBUTE_NODE( "AngularDamping", "Value", m_angularDamping );
			XML_CASE_ATTRIBUTE_NODE( "AllowSleep", "Value", m_allowSleep );
			XML_CASE_ATTRIBUTE_NODE( "IsBullet", "Value", m_isBullet );
			XML_CASE_ATTRIBUTE_NODE( "FixedRotation", "Value", m_fixedRotation );
			XML_CASE_ATTRIBUTE_NODE( "IsSensor", "Value", m_isSensor );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::loaderShape_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			mt::vec2f point;
			XML_CASE_NODE( "Point" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{					
					XML_CASE_ATTRIBUTE( "Value", point );
				}
				m_shapeList.back().add_point( point );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::loaderShapeCircle_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			float radius;
			mt::vec2f pos;

			XML_CASE_NODE( "Radius" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{					
					XML_CASE_ATTRIBUTE( "Value", radius );
				}
				m_shapeCircleList.back().first = radius;
			}

			XML_CASE_NODE( "Position" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{					
					XML_CASE_ATTRIBUTE( "Value", pos );
				}
				m_shapeCircleList.back().second = pos;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::loaderShapeBox_( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			float width;
			float height;
			float angle;
			mt::vec2f pos;

			XML_CASE_NODE( "Width" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{					
					XML_CASE_ATTRIBUTE( "Value", width );
				}
				m_shapeBoxList.back().first.first = width;
			}

			XML_CASE_NODE( "Height" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{					
					XML_CASE_ATTRIBUTE( "Value", height );
				}
				m_shapeBoxList.back().first.second = height;
			}

			XML_CASE_NODE( "Position" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{					
					XML_CASE_ATTRIBUTE( "Value", pos );
				}
				m_shapeBoxList.back().second.first = pos;
			}

			XML_CASE_NODE( "Angle" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{					
					XML_CASE_ATTRIBUTE( "Value", angle );
				}
				m_shapeBoxList.back().second.second = angle;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_update( float _timing )
	{
		if( !(m_interface->isFrozen() || m_interface->isStatic() || m_interface->isSleeping()) )
		{
			const float phase = Holder<PhysicEngine2D>::hostage()->getPhase();

			const float * pos = m_interface->getPosition();
			const mt::vec2f& prevPos = getLocalPosition();
			mt::vec2f currPos( pos[0], pos[1] );
			//currPos = currPos * phase + prevPos * ( 1.0f - phase );
			setLocalPosition( currPos );

			const float * orient = m_interface->getOrientation();
			setLocalDirection( mt::vec2f( orient[0], orient[1] ) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		if( m_interface == 0 )
		{
			return;
		}

		const mt::vec2f & wposition = getWorldPosition();

		m_interface->setPosition( wposition.x, wposition.y );

		const mt::vec2f & direction = getWorldDirection();

		float angle = mt::signed_angle( direction );

		m_interface->setOrientation( angle );
	}
	//////////////////////////////////////////////////////////////////////////
	/*bool RigidBody2D::_activate()
	{
		if( m_interface == 0 ) // maybe just deactivated, try to compile
		{
			_compile();
		}

		return true;
	}*/
	//////////////////////////////////////////////////////////////////////////
	/*void RigidBody2D::_deactivate()
	{
		Holder<PhysicEngine2D>::hostage()->destroyPhysicBody( m_interface );
		m_interface = 0;
	}*/
	//////////////////////////////////////////////////////////////////////////
	bool RigidBody2D::_compile()
	{
		const mt::vec2f & position = getWorldPosition();

		m_interface = Holder<PhysicEngine2D>::hostage()->createBody( position, 0.0f, m_linearDamping, m_angularDamping, m_allowSleep,
																			m_isBullet, m_fixedRotation );

		if( m_interface == 0 )
		{
			return false;
		}

		m_bodyListener = new BodyListenerProxy( this );
		m_interface->setBodyListener( m_bodyListener );
		m_interface->setUserData( this );

		compileShapes_();

		for( TShapeList::iterator it = m_shapeList.begin(),
				it_end = m_shapeList.end();
				it != it_end;
				it++ )
		{
			std::size_t numPoint = it->num_points();

			const mt::TVectorPoints & vectorPoints = it->get_points();

			m_interface->addShapeConvex( 
				numPoint, 
				vectorPoints.front().buff(), 
				m_density, 
				m_friction, 
				m_restitution, 
				m_isSensor,
				m_collisionMask, 
				m_categoryBits, 
				m_groupIndex );

		}

		for( TShapeCircleList::iterator it = m_shapeCircleList.begin(),
			it_end = m_shapeCircleList.end();
			it != it_end;
			it++ )
		{
			m_interface->addShapeCircle( it->first, it->second.buff(), m_density, m_friction, m_restitution, m_isSensor,
											m_collisionMask, m_categoryBits, m_groupIndex );
		}

		for( TShapeBoxList::iterator it = m_shapeBoxList.begin(),
			it_end = m_shapeBoxList.end();
			it != it_end;
			it++ )
		{
			m_interface->addShapeBox( it->first.first, it->first.second, it->second.first.buff(), it->second.second, m_density, m_friction, m_restitution, m_isSensor,
										m_collisionMask, m_categoryBits, m_groupIndex );
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_release()
	{
		Holder<PhysicEngine2D>::hostage()->destroyPhysicBody( m_interface );
		m_interface = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::applyForce( float _forceX, float _forceY, float _pointX, float _pointY )
	{
		const mt::vec2f & position = getWorldPosition();

		m_interface->applyForce( _forceX, _forceY, position.x + _pointX, position.y + _pointY );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::applyImpulse( float _impulseX, float _impulseY, float _pointX, float _pointY )
	{
		const mt::vec2f & position = getWorldPosition();

		m_interface->applyImpulse( _impulseX, _impulseY, position.x + _pointX, position.y + _pointY );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::applyConstantForce( float _forceX, float _forceY, float _pointX, float _pointY )
	{
		m_force.x = _forceX;
		m_force.y = _forceY;
		m_forcePoint.x = _pointX;
		m_forcePoint.y = _pointY;
		m_constantForce = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::removeConstantForce()
	{
		m_constantForce = false;
	}
	//////////////////////////////////////////////////////////////////////////
	PhysicBody2DInterface * RigidBody2D::getInterface()
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::setDirectionForce( bool _relative )
	{
		m_directionForce = _relative;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::wakeUp()
	{
		if( m_interface )
		{
			m_interface->wakeUp();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float RigidBody2D::getMass() const
	{
		return m_interface->getMass();
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f RigidBody2D::getLinearVelocity() const
	{
		const float * v = m_interface->getLinearVelocity();
		return mt::vec2f( v[0], v[1] );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::setLinearVelocity( float _x, float _y, bool _countGravity )
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
	void RigidBody2D::_addShapeBox( float _width, float _heigth, const mt::vec2f& _pos, float _angle )
	{
		m_shapeBoxList.push_back( std::make_pair( std::make_pair( _width, _heigth ), std::make_pair( _pos, _angle ) ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_addShapeConvex( const mt::TVectorPoints & _points, bool _isSensor )
	{
		/*m_interface->addShapeConvex( 
		_points.size(), 
		_points.front().m, 
		m_density, 
		m_friction, 
		m_restitution, 
		_isSensor,
		m_collisionMask, 
		m_categoryBits, 
		m_groupIndex );*/
		mt::polygon poly;
		for( mt::TVectorPoints::size_type i = 0; i < _points.size(); i++ )
		{
			poly.add_point( _points[i] );
		}
		m_shapeList.push_back( poly );
		m_isSensor = _isSensor;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_setListener()
	{
		Node::_setListener();
		
		Eventable::registerEvent( EVENT_COLLIDE, ("onCollide"), m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::onApplyForceAndTorque()
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
			const float* v = m_interface->getLinearVelocity();
			mt::vec2f cv( v[0], v[1] );
			mt::vec2f force = ( m_velocity - cv ) * m_interface->getMass() * 60.0f;
			if( m_countGravity )
			{
				force -= Holder<PhysicEngine2D>::hostage()->getGravity() * m_interface->getMass();
			}
			const float* pos = m_interface->getPosition();
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
	void RigidBody2D::_render( Camera2D * _camera )
	{
		Node::_render( _camera );
#	ifndef MENGE_MASTER_RELEASE

		unsigned int debugMask = Application::hostage()->getDebugMask();

		if( debugMask & MENGE_DEBUG_PHYSICS )
		{
			for( TShapeList::iterator it = m_shapeList.begin(),
				it_end = m_shapeList.end();
				it != it_end;
			it++ )
			{
				const mt::polygon & poly = *it;
				const mt::mat3f& mtx = getWorldMatrix();
				//mt::vec2f pos = getLocalPosition();

				std::size_t numPoints = poly.num_points();

				for(std::size_t i = 0; i != numPoints; i++)
				{
					
					mt::vec2f beg = poly[i];
					mt::vec2f end = poly[(i+1) % numPoints];

					//beg += pos;
					//end += pos;
					mt::vec2f pt1, pt2;
					mt::mul_v2_m3( pt1, beg, mtx );
					mt::mul_v2_m3( pt2, end, mtx );

					//Holder<RenderEngine>::hostage()->renderLine(0xFFFFFFFF,pt1,pt2);
				}
			}

			for( TShapeBoxList::iterator itb = m_shapeBoxList.begin(),
				itb_end = m_shapeBoxList.end();
				itb != itb_end;
			itb++ )
			{
				/*float width = itb->first.first;
				float height = itb->first.second;
				mt::vec2f pos = itb->second.first;
				float angle = itb->second.second;

				const mt::mat3f & mtx = getWorldMatrix();

				mt::vec2f temp0( -width * 0.5f, -height * 0.5f );
				mt::vec2f temp1( width * 0.5f, height * 0.5f );
				mt::vec2f pts[2];

				mt::mul_v2_m3( pts[0], temp0, mtx );
				mt::mul_v2_m3( pts[1], temp1, mtx );

				mt::vec2f pts[4];

				for( int i = 0; i < 4; i++ )
				{
					mt::mul_v2_m3( pts[i], temp[i], mtx );
				}*/

				//reng->renderLine(0xFFFFFFFF, pts[0], pts[1] );
				//reng->renderLine(0xFFFFFFFF, pts[0], pts[3] );
				//reng->renderLine(0xFFFFFFFF, pts[1], pts[2] );
				//reng->renderLine(0xFFFFFFFF, pts[3], pts[2] );

			}
		}
#	endif
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::unsetLinearVelocity()
	{
		if( m_linearVelocity )
		{
			m_unsetLinearVelocity = true;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::freeze( bool _freeze )
	{
		if( m_frozen == _freeze )
		{
			return;
		}
		m_frozen = _freeze;

		if( m_frozen )	// completly remove from simulation
		{
			_release();
		}
		else
		{
			_compile();
		}

		m_updatable = !_freeze;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::setCollisionMask( int _mask )
	{
		m_collisionMask = (uint16)_mask;
		_updateFilterData();
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_updateFilterData()
	{
		m_interface->updateFilterData( m_categoryBits, m_collisionMask, m_groupIndex );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::onUpdate()
	{
		onApplyForceAndTorque();

		const float * pos = m_interface->getPosition();
		mt::vec2f currPos( pos[0], pos[1] );
		setLocalPosition( currPos );

		const float * orient = m_interface->getOrientation();
		setLocalDirection( mt::vec2f( orient[0], orient[1] ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::compileShapes_()
	{
		/*for( int i = 0; i < m_shapeList.size(); )
		{
			if( m_shapeList[i].num_points() == 4 )
			{
				const mt::TVectorPoints& pts = m_shapeList[i].get_points();
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
	void RigidBody2D::_updateBoundingBox( mt::box2f & _boundingBox )
	{
		Node::_updateBoundingBox( _boundingBox );

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
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::enableStabilization( bool _enable, float _stabilityAngle, float _stabilityForce )
	{
		m_stabilization = _enable;
		m_stabilityAngle = _stabilityAngle;
		m_stabilityForce = _stabilityForce;
	}
	//////////////////////////////////////////////////////////////////////////

}
