#	include "ResourcePhysicConcaveGeometry.h"

#	include "ResourceImplement.h"

#	include "PhysicEngine.h"

#	include "LogEngine.h"

#	include "XmlEngine.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourcePhysicConcaveGeometry )
	//////////////////////////////////////////////////////////////////////////
	ResourcePhysicConcaveGeometry::ResourcePhysicConcaveGeometry( const std::string & _name )
		: ResourcePhysicGeometry( _name )
		, m_interface(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePhysicConcaveGeometry::loader( XmlElement * _xml )
	{
		ResourcePhysicGeometry::loader( _xml );
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "ConcaveGeometryFile", "Name", m_filename );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePhysicConcaveGeometry::_compile()
	{
		m_interface = Holder<PhysicEngine>::hostage()->cookConcave( m_filename );

		if( m_interface == 0 )
		{
			MENGE_LOG("Warning: resource '%s' can't load concave geometry file '%s'\n"
				, m_name.c_str()
				, m_filename.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePhysicConcaveGeometry::_release()
	{
		Holder<PhysicEngine>::hostage()->removeGeometry( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	const GeometryInterface * ResourcePhysicConcaveGeometry::getGeometry( size_t _index ) const
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
}