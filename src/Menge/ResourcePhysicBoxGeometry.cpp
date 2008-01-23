#	include "ResourcePhysicBoxGeometry.h"

#	include "ResourceImplement.h"

#	include "PhysicEngine.h"

#	include "LogEngine.h"

#	include "XmlEngine.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourcePhysicBoxGeometry )
	//////////////////////////////////////////////////////////////////////////
	ResourcePhysicBoxGeometry::ResourcePhysicBoxGeometry( const std::string & _name )
		: ResourcePhysicGeometry( _name )
		, m_width(0.0f)
		, m_height(0.0f)
		, m_depth(0.0f)
		, m_interface(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePhysicBoxGeometry::loader( XmlElement * _xml )
	{
		ResourcePhysicGeometry::loader( _xml );
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "Width", "Value", m_width );
			XML_CASE_ATTRIBUTE_NODE( "Height", "Value", m_height );
			XML_CASE_ATTRIBUTE_NODE( "Depth", "Value", m_depth );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePhysicBoxGeometry::_compile()
	{
		m_interface = Holder<PhysicEngine>::hostage()->cookBox( m_width, m_height, m_depth );

		if( m_interface == 0 )
		{
			MENGE_LOG("Warning: resource '%s' can't cook box geometry\n"
				, m_name.c_str()
			);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePhysicBoxGeometry::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const GeometryInterface * ResourcePhysicBoxGeometry::getGeometry( size_t _index ) const
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
}