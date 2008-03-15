#	include "RigidBody3D.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"
#	include "PhysicEngine.h"

#	include "ResourcePhysicGeometry.h"

#	include "ResourceManager.h"

#	include "LogEngine.h"

#	include "XmlEngine.h"

#	include "SceneNode3D.h"

#	include "Interface/RenderSystemInterface.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( RigidBody3D )
	//////////////////////////////////////////////////////////////////////////
	RigidBody3D::RigidBody3D()
		: m_active(true)
		, m_density(0.0f)
		, m_physInterface(0)
		, m_resource(0)
	{}	
	//////////////////////////////////////////////////////////////////////////
	RigidBody3D::~RigidBody3D()
	{}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::_update( float _timing )
	{
		const float* pos = m_physInterface->getPosition();
		m_interface->setLocalPosition( pos );

		const float* orient = m_physInterface->getOrient();
		m_interface->setLocalOrient( orient );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::loader( XmlElement * _xml )
	{
		SceneNode3D::loader(_xml); // надо?

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
		SceneNode3D::_compile();

		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourcePhysicGeometry>( m_resourcename );

		if( m_resource == 0 )
		{
			return false;
		}

		const GeometryInterface * geometry = m_resource->getGeometry(0);

		m_physInterface = Holder<PhysicEngine>::hostage()->createRigidBody( m_density, m_active, geometry );

		const mt::vec3f & position = getWorldPosition();
		m_physInterface->setPosition(position.x, position.y, position.z);

		const mt::quatf & orient = getWorldOrient();
		m_physInterface->setOrient(orient.w,orient.x,orient.y,orient.z);
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::_release()
	{
		Holder<ResourceManager>::hostage()
			->releaseResource( m_resource );

		m_resource = 0;

		Holder<PhysicEngine>::hostage()->removeRigidBody( m_physInterface );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::applyImpulse( const mt::vec3f & _vec )
	{
		m_physInterface->applyImpulse( _vec.x, _vec.y, _vec.z );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::applyAngularImpulse( const mt::vec3f & _vec )
	{
		m_physInterface->applyAngularImpulse( _vec.x, _vec.y, _vec.z );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::applyForce( const mt::vec3f & _vec )
	{
		m_physInterface->applyForce( _vec.x, _vec.y, _vec.z );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::applyTorque( const mt::vec3f & _vec )
	{
		m_physInterface->applyTorque( _vec.x, _vec.y, _vec.z );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::setLinearVelocity( const mt::vec3f & _vec )
	{
		m_physInterface->setLinearVelocity( _vec.x, _vec.y, _vec.z );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::setAngularVelocity( const mt::vec3f & _vec )
	{
		m_physInterface->setAngularVelocity( _vec.x, _vec.y, _vec.z );
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::setActive( bool _active )
	{
		m_physInterface->setActive( _active );
	}
	//////////////////////////////////////////////////////////////////////////
}