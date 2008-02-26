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
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RigidBody2D::~RigidBody2D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::setPosition( const mt::vec2f& _pos )
	{
		m_interface->setPosition( _pos.x, _pos.y );
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f RigidBody2D::getPosition() const
	{
		const float* pos = m_interface->getPosition();
		return mt::vec2f( pos[0], pos[1] );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::onCollide( PhysicBody2DInterface* _otherObj, float _worldX, float _worldY, float _normalX, float _normalY )
	{
		printf( "onCollide!!!!\n" );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::loader( XmlElement * _xml )
	{
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
	void RigidBody2D::render()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	bool RigidBody2D::isRenderable()
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_update( float _timing )
	{
		m_interface->setPosition( getWorldPosition().x, getWorldPosition().y );
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
		m_interface = Holder<PhysicEngine2D>::hostage()->createPhysicBody( m_shape, mt::vec2f(0.0f, 0.0f), m_density, m_friction, m_restitution );
		m_interface->setCollisionListener( this );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody2D::_release()
	{
		Holder<PhysicEngine2D>::hostage()->destroyPhysicBody( m_interface );
	}

} // namespace Menge