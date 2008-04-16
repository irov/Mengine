#	include "ObjectImplement.h"

#	include "Math/vec2.h"
#	include "RigidBody2D.h"

#	include "PhysicEngine2D.h"
#	include "RenderEngine.h"
#	include "XmlEngine.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	void CollisionListenerProxy::onCollide(PhysicBody2DInterface *_otherObj, float _worldX, float _worldY, float _normalX, float _normalY)
	{
		m_body->onCollide( _otherObj, _worldX, _worldY, _normalX, _normalY );
	}
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( RigidBody2D )
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
	, m_collisionListenerProxy( NULL )
	, m_isSensor( false )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RigidBody2D::~RigidBody2D()
	{
		if( m_collisionListenerProxy )
		{
			delete m_collisionListenerProxy;
			m_collisionListenerProxy = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::onCollide( PhysicBody2DInterface* _otherObj, float _worldX, float _worldY, float _normalX, float _normalY )
	{
		if( m_listener )
		{
			RigidBody2D* other = static_cast<RigidBody2D*>( _otherObj->getUserData() );
			callEvent( "ON_COLLIDE", "(OOffff)", this->getScript(), other->getScript(), _worldX, _worldY, _normalX, _normalY );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::loader( XmlElement * _xml )
	{
		SceneNode2D::loader( _xml );
		_loaderPhysics( _xml );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_loaderPhysics( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Shape" )
			{
				mt::polygon n;
				m_shapeList.push_back( n );
				XML_PARSE_ELEMENT( this, &RigidBody2D::_loaderShape );
			}
			XML_CASE_NODE( "ShapeCircle" )
			{
				std::pair< float, mt::vec2f > n;
				m_shapeCircleList.push_back( n );
				XML_PARSE_ELEMENT( this, &RigidBody2D::_loaderShapeCircle );
			}
			XML_CASE_NODE( "ShapeBox" )
			{
				std::pair< std::pair< float, float >, std::pair< mt::vec2f, float > > n;
				m_shapeBoxList.push_back( n );
				XML_PARSE_ELEMENT( this, &RigidBody2D::_loaderShapeBox );
			}

			XML_CASE_ATTRIBUTE_NODE("Density", "Value", m_density );
			XML_CASE_ATTRIBUTE_NODE("Friction", "Value", m_friction );
			XML_CASE_ATTRIBUTE_NODE("Restitution", "Value", m_restitution );
			XML_CASE_ATTRIBUTE_NODE("CollisionMask", "Value", m_collisionMask );
			XML_CASE_ATTRIBUTE_NODE("CategoryBits", "Value", m_categoryBits );
			XML_CASE_ATTRIBUTE_NODE("GroupIndex", "Value", m_groupIndex );
			XML_CASE_ATTRIBUTE_NODE("LinearDamping", "Value", m_linearDamping );
			XML_CASE_ATTRIBUTE_NODE("AngularDamping", "Value", m_angularDamping );
			XML_CASE_ATTRIBUTE_NODE("AllowSleep", "Value", m_allowSleep );
			XML_CASE_ATTRIBUTE_NODE("IsBullet", "Value", m_isBullet );
			XML_CASE_ATTRIBUTE_NODE("FixedRotation", "Value", m_fixedRotation );
			XML_CASE_ATTRIBUTE_NODE("IsSensor", "Value", m_isSensor );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_loaderShape( XmlElement * _xml )
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
	void RigidBody2D::_loaderShapeCircle( XmlElement * _xml )
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
	void RigidBody2D::_loaderShapeBox( XmlElement * _xml )
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

			const float* pos = m_interface->getPosition();
			setLocalPosition( mt::vec2f( pos[0], pos[1] ) );
			const float* orient = m_interface->getOrientation();
			setLocalDirection( mt::vec2f( orient[0], orient[1] ) );

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

				mt::mul_v2_m3( point, m_forcePoint, getWorldMatrix() );

				m_interface->applyForce( force.x, force.y, point.x, point.y );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RigidBody2D::_activate()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_deactivate()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool RigidBody2D::_compile()
	{
		const mt::vec2f & position = getWorldPosition();
		if( m_density == 0.0f )
		{
			m_interface = Holder<PhysicEngine2D>::hostage()->createStaticBody( position, 0.0f );
		}
		else
		{
			m_interface = Holder<PhysicEngine2D>::hostage()->createDynamicBody( position, 0.0f, m_linearDamping, m_angularDamping, m_allowSleep,
																			m_isBullet, m_fixedRotation );
		}

		m_collisionListenerProxy = new CollisionListenerProxy( this );
		m_interface->setCollisionListener( m_collisionListenerProxy );
		m_interface->setUserData( this );

		for( TShapeList::iterator it = m_shapeList.begin(),
				it_end = m_shapeList.end();
				it != it_end;
				it++ )
		{
			m_interface->addShapeConvex( (*it).num_points(), &(((*it)[0]).x), m_density, m_friction, m_restitution, m_isSensor,
											m_collisionMask, m_categoryBits, m_groupIndex );
		}

		for( TShapeCircleList::iterator it = m_shapeCircleList.begin(),
			it_end = m_shapeCircleList.end();
			it != it_end;
			it++ )
		{
			m_interface->addShapeCircle( it->first, it->second.m, m_density, m_friction, m_restitution, m_isSensor,
											m_collisionMask, m_categoryBits, m_groupIndex );
		}

		for( TShapeBoxList::iterator it = m_shapeBoxList.begin(),
			it_end = m_shapeBoxList.end();
			it != it_end;
			it++ )
		{
			m_interface->addShapeBox( it->first.first, it->first.second, it->second.first.m, it->second.second, m_density, m_friction, m_restitution, m_isSensor,
										m_collisionMask, m_categoryBits, m_groupIndex );
		}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_release()
	{
		Holder<PhysicEngine2D>::hostage()->destroyPhysicBody( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	/*void RigidBody2D::setListener( PyObject * _listener )
	{
		m_scriptListener = _listener;

		registerEventListener("ON_COLLIDE", "onCollide", m_scriptListener );
	}*/
	//////////////////////////////////////////////////////////////////////////
	PyObject* RigidBody2D::getListener()
	{
		return m_listener;
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
	void RigidBody2D::setAngle( float _angle )
	{
		m_interface->setOrientation( _angle );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::setDirection( const mt::vec2f& _dir )
	{
		float sign = _dir.y < 0.f ? -1.f : 1.f;
		float cos = _dir.x;
		if( cos < -1.0f ) cos = -1.0f;
		else if( cos > 1.0f ) cos = 1.0f;
		float angle = ::acos( cos ) * sign;
		//printf( "setAngle %.2f\n", angle);
		m_interface->setOrientation( angle );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::setPosition( float _x, float _y )
	{
		const mt::vec2f & position = getWorldPosition();
		m_interface->setPosition( _x, _y );
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
	PhysicBody2DInterface* RigidBody2D::getInterface()
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
		m_interface->wakeUp();
	}
	//////////////////////////////////////////////////////////////////////////
	float RigidBody2D::getMass() const
	{
		return m_interface->getMass();
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f RigidBody2D::getLinearVelocity() const
	{
		const float* v = m_interface->getLinearVelocity();
		return mt::vec2f( v[0], v[1] );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_addShapeBox( float _width, float _heigth, const mt::vec2f& _pos, float _angle )
	{
		m_shapeBoxList.push_back( std::make_pair( std::make_pair( _width, _heigth ), std::make_pair( _pos, _angle ) ) );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_onSetListener()
	{
		SceneNode2D::_onSetListener();

		registerEventListener("ON_COLLIDE", "onCollide", m_listener );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_debugRender()
	{
		for( TShapeList::iterator it = m_shapeList.begin(),
			it_end = m_shapeList.end();
			it != it_end;
		it++ )
		{
			const mt::polygon & poly = *it;

			for(int i = 0; i < poly.num_points(); i++)
			{
				mt::vec2f beg = poly[i];
				mt::vec2f end = poly[(i+1) % poly.num_points()];

				beg+=getWorldPosition();
				end+=getWorldPosition();

				Holder<RenderEngine>::hostage()->renderLine(0xFFFFFFFF,beg,end);
			}
		}
	}
} // namespace Menge