#	include "ResourceCapsuleController.h"

#	include "ResourceImplement.h"

#	include "PhysicEngine.h"

#	include "LogEngine.h"

#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceCapsuleController );
	//////////////////////////////////////////////////////////////////////////
	ResourceCapsuleController::ResourceCapsuleController( const ResourceFactoryParam & _params )
		: ResourceReference( _params )
		, m_radius(1.f)
		, m_height(1.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceCapsuleController::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE("Radius", "Value", m_radius );					
			XML_CASE_ATTRIBUTE_NODE("Height", "Value", m_height );					
		}
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceCapsuleController::getRadius()
	{
		return m_radius;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceCapsuleController::getHeight()
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceCapsuleController::_compile()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceCapsuleController::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
}