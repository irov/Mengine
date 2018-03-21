//#	include "CapsuleController.h"
//#	include "ObjectImplement.h"
//
//#	include "ScriptEngine.h"
//#	include "PhysicEngine.h"
//
//#	include "ResourceCapsuleController.h"
//
//#	include "ResourceManager.h"
//
//#	include "LogEngine.h"
//
//#	include "XmlEngine.h"
//
//#	include "SceneNode3D.h"
//
//namespace	Menge
//{
//	//////////////////////////////////////////////////////////////////////////
//	OBJECT_IMPLEMENT( CapsuleController )
//	//////////////////////////////////////////////////////////////////////////
//	CapsuleController::CapsuleController()
//		: m_interface(0)
//		, m_startPosition(0.f, 0.f, 0.f)
//		, m_radius(1.f)
//		, m_height(1.f)
//		//, m_resource(0)
//	{}	
//	//////////////////////////////////////////////////////////////////////////
//	CapsuleController::~CapsuleController()
//	{}
//	//////////////////////////////////////////////////////////////////////////
//	void CapsuleController::move( const mt::vec3f & _displacement )
//	{
//		m_interface->move( _displacement.x, _displacement.y, _displacement.z );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	const mt::vec3f & CapsuleController::getFilteredPosition()
//	{
//		return *(mt::vec3f*)m_interface->getFilteredPosition();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void CapsuleController::setPosition( const mt::vec3f & _pos )
//	{
//		m_interface->setPosition( _pos.x, _pos.y, _pos.z );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void CapsuleController::_update( float _timing )
//	{
//		assert(0);
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void CapsuleController::loader( XmlElement * _xml )
//	{
//		XML_SWITCH_NODE( _xml )
//		{
//			XML_CASE_ATTRIBUTE_NODE("StartPosition", "Value", m_startPosition );					
//			XML_CASE_ATTRIBUTE_NODE("Resource", "Name", m_resourcename );					
//		}
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool CapsuleController::_activate()
//	{
//		return true;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void CapsuleController::_deactivate()
//	{
//	}
//	//////////////////////////////////////////////////////////////////////////
//	bool CapsuleController::_compile()
//	{
//		m_resource = 
//			Holder<ResourceManager>::hostage()
//			->getResourceT<ResourceCapsuleController>( m_resourcename );
//
//		if( m_resource == NULL )
//		{
//			MENGE_LOG_ERROR( "Error: Emitter can't open resource file '%s'\n", m_resourcename.c_str() );
//			return false;
//		}
//
//		m_interface = Holder<PhysicEngine>::hostage()->createCapsuleController(
//			m_startPosition, m_resource->getRadius(), m_resource->getHeight()
//			);
//
//		if( m_interface == 0 )
//		{
//			return false;
//		}
//
//		return true;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void CapsuleController::_release()
//	{
//		Holder<ResourceManager>::hostage()
//			->releaseResource( m_resource );
//
//		m_resource = 0;
//
//		Holder<PhysicEngine>::hostage()->releaseCapsuleController( m_interface );
//	}
//	//////////////////////////////////////////////////////////////////////////
//}