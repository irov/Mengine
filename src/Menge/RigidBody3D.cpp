#	include "RigidBody3D.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"
#	include "PhysicEngine.h"

#	include "ResourcePhysicGeometry.h"

#	include "ResourceManager.h"

#	include "LogEngine.h"

#	include "XmlEngine.h"

#	include "SceneNode3D.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( RigidBody3D )
	//////////////////////////////////////////////////////////////////////////
	RigidBody3D::RigidBody3D()
		: m_active(true)
		, m_density(0.0f)
		, m_interface(0)
		, m_resource(0)
	{}	
	//////////////////////////////////////////////////////////////////////////
	RigidBody3D::~RigidBody3D()
	{}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::setPosition( const mt::vec3f & _position )
	{
		//m_interface->setPosition( _position.x, _position.y, _position.z );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::setOrientation( const mt::quatf & _quat )
	{
		//m_interface->setOrient( _quat.w, _quat.x, _quat.y, _quat.z );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::_update( float _timing )
	{
		assert(0);
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & RigidBody3D::getPosition()
	{
		return *(mt::vec3f*)m_interface->getPosition();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::quatf & RigidBody3D::getOrientation()
	{
		return *(mt::quatf*)m_interface->getOrient();
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE("GeometryResource", "Name", m_resourcename );
			XML_CASE_ATTRIBUTE_NODE("Active", "Value", m_active );					
			XML_CASE_ATTRIBUTE_NODE("Density", "Value", m_density );					
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool RigidBody3D::_activate()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::_deactivate()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool RigidBody3D::_compile()
	{
		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourcePhysicGeometry>( m_resourcename );

		if( m_resource == 0 )
		{
			return false;
		}

		const GeometryInterface * geometry = m_resource->getGeometry(0);

		m_interface = Holder<PhysicEngine>::hostage()->createRigidBody( m_density, m_active, geometry );
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::_release()
	{
		Holder<ResourceManager>::hostage()
			->releaseResource( m_resource );

		m_resource = 0;

		Holder<PhysicEngine>::hostage()->removeRigidBody( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::applyImpulse( const mt::vec3f & _vec )
	{
		m_interface->applyImpulse( _vec.x, _vec.y, _vec.z );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::applyAngularImpulse( const mt::vec3f & _vec )
	{
		m_interface->applyAngularImpulse( _vec.x, _vec.y, _vec.z );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::applyForce( const mt::vec3f & _vec )
	{
		m_interface->applyForce( _vec.x, _vec.y, _vec.z );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::applyTorque( const mt::vec3f & _vec )
	{
		m_interface->applyTorque( _vec.x, _vec.y, _vec.z );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::setLinearVelocity( const mt::vec3f & _vec )
	{
		m_interface->setLinearVelocity( _vec.x, _vec.y, _vec.z );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::setAngularVelocity( const mt::vec3f & _vec )
	{
		m_interface->setAngularVelocity( _vec.x, _vec.y, _vec.z );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::setActive( bool _active )
	{
		m_interface->setActive( _active );
	}
	//////////////////////////////////////////////////////////////////////////
}