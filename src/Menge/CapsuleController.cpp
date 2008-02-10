#	include "CapsuleController.h"
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
	OBJECT_IMPLEMENT( CapsuleController )
	//////////////////////////////////////////////////////////////////////////
	CapsuleController::CapsuleController()
		: m_interface(0)
		//, m_resource(0)
	{}	
	//////////////////////////////////////////////////////////////////////////
	CapsuleController::~CapsuleController()
	{}
	//////////////////////////////////////////////////////////////////////////
	void CapsuleController::attachSceneNode( SceneNode3D * _node )
	{
		assert(_node);
		_node->attachMotionModifier( this );
	}
	//////////////////////////////////////////////////////////////////////////
	void CapsuleController::setPosition( const mt::vec3f & _position )
	{
		m_interface->setPosition( (float*)_position.m );
	}
	//////////////////////////////////////////////////////////////////////////
	void CapsuleController::_update( float _timing )
	{
		assert(0);
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec3f & CapsuleController::getPosition()
	{
		return *(mt::vec3f*)m_interface->getFilteredPosition();
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::quatf & CapsuleController::getOrientation()
	{
		static mt::quatf q(1,0,0,0);
		return	q;
	}
	//////////////////////////////////////////////////////////////////////////
	void CapsuleController::loader( XmlElement * _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE("Radius", "Value", m_radius );					
			XML_CASE_ATTRIBUTE_NODE("Height", "Value", m_height );					
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool CapsuleController::_activate()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void CapsuleController::_deactivate()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool CapsuleController::_compile()
	{
	/*	m_resource = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourcePhysicGeometry>( m_resourcename );

		if( m_resource == 0 )
		{
			return false;
		}
*/
		//m_interface = Holder<PhysicEngine>::hostage()->createCapsuleController( m_density, m_radius, m_height );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void CapsuleController::_release()
	{
		//Holder<ResourceManager>::hostage()
		//	->releaseResource( m_resource );

		//m_resource = 0;

		//Holder<PhysicEngine>::hostage()->removeRigidBody( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
}