#	include "ResourcePhysicConvexGeometry.h"

#	include "ResourceImplement.h"

#	include "PhysicEngine.h"

#	include "LogEngine.h"

#	include "XmlEngine.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourcePhysicConvexGeometry )
	//////////////////////////////////////////////////////////////////////////
	ResourcePhysicConvexGeometry::ResourcePhysicConvexGeometry( const std::string & _name )
		: ResourcePhysicGeometry( _name )
		, m_interface(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePhysicConvexGeometry::loader( XmlElement * _xml )
	{
		ResourcePhysicGeometry::loader( _xml );
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "ConvexGeometryFile", "Name", m_filename );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePhysicConvexGeometry::_compile()
	{
		m_interface = Holder<PhysicEngine>::hostage()->cookConvex( m_filename );

		if( m_interface == 0 )
		{
			MENGE_LOG("Warning: resource '%s' can't load convex geometry file '%s'\n"
				, m_name.c_str()
				, m_filename.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePhysicConvexGeometry::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const GeometryInterface * ResourcePhysicConvexGeometry::getGeometry( size_t _index ) const
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
}