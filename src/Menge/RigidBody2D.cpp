#	include "ObjectImplement.h"

#	include "Math/vec2.h"
#	include "RigidBody2D.h"

#	include "PhysicEngine2D.h"
#	include "XmlEngine.h"


namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( RigidBody2D )
	//////////////////////////////////////////////////////////////////////////
	RigidBody2D::RigidBody2D()
	: m_interface( NULL )
	, m_density( 1.0f )
	, m_friction( 1.0f )
	, m_restitution( 0.0f )
	, m_scriptListener( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RigidBody2D::~RigidBody2D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::onCollide( PhysicBody2DInterface* _otherObj, float _worldX, float _worldY, float _normalX, float _normalY )
	{
		if( m_scriptListener )
		{
			callEvent( "ON_COLLIDE", "(OOffff)", this->getScript(), _otherObj->getUserData(), _worldX, _worldY, _normalX, _normalY );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::loader( XmlElement * _xml )
	{
		//SceneNode2D::loader( _xml );
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Shape" )
			{
				XML_PARSE_ELEMENT( this, &RigidBody2D::_loaderShape );
			}

			XML_CASE_ATTRIBUTE_NODE("Density", "Value", m_density );
			XML_CASE_ATTRIBUTE_NODE("Friction", "Value", m_friction );
			XML_CASE_ATTRIBUTE_NODE("Restitution", "Value", m_restitution );
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
				m_shape.add_point( point );
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
		m_interface = Holder<PhysicEngine2D>::hostage()->createPhysicBody( m_shape, getWorldPosition(), m_density, m_friction, m_restitution );
		m_interface->setCollisionListener( this );
		m_interface->setUserData( this );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_release()
	{
		Holder<PhysicEngine2D>::hostage()->destroyPhysicBody( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::setListener( PyObject * _listener )
	{
		m_scriptListener = _listener;

		registerEventListener("ON_COLLIDE", "onCollide", m_scriptListener );
	}
	//////////////////////////////////////////////////////////////////////////
	PyObject* RigidBody2D::getListener()
	{
		return m_scriptListener;
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
		m_interface->applyImpulse( _impulseX, _impulseY, _pointX, _pointY );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::setOrientation( float _angle )
	{
		m_interface->setOrientation( _angle );
	}
} // namespace Menge