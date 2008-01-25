#	include "RigidBody3D.h"
#	include "ObjectImplement.h"

#	include "ScriptEngine.h"
#	include "RenderEngine.h"
#	include "PhysicEngine.h"

#	include "ResourcePhysicGeometry.h"

#	include "ResourceManager.h"

#	include "LogEngine.h"

#	include "XmlEngine.h"


namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( RigidBody3D )
	//////////////////////////////////////////////////////////////////////////
	RigidBody3D::RigidBody3D()
		: m_active(true)
		, m_density(0.0f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RigidBody3D::~RigidBody3D()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::_update( float _timing )
	{
		float * R = m_interface->getOrient();
		this->setLocalOrient(*(mt::quatf *)R);

		float * pos = m_interface->getPosition();
		this->setLocalPosition(*(mt::vec3f *)pos);
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::loader( XmlElement * _xml )
	{
		SceneNode3D::loader(_xml);

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
		if( SceneNode3D::_activate() == false )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::_deactivate()
	{
		SceneNode3D::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	bool RigidBody3D::_compile()
	{
		if( SceneNode3D::_compile() == false )
		{
			return false;
		}

		m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourcePhysicGeometry>( m_resourcename );

		const GeometryInterface * geometry = m_resource->getGeometry(0);

		m_interface = Holder<PhysicEngine>::hostage()->createRigidBody( m_density, m_active, geometry );

		const mt::vec3f & pos = getLocalPosition();

		m_interface->setPosition(pos.x, pos.y, pos.z);

		const mt::quatf & orient = getLocalOrient();

		m_interface->setOrient(orient.w, orient.x, orient.y, orient.z);

		if( m_resource == 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RigidBody3D::_release()
	{
		SceneNode3D::_release();

		Holder<PhysicEngine>::hostage()->removeRigidBody( m_interface );

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resource );

		m_resource = 0;
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