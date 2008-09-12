#	include "ResourcePhysicConvexGeometry.h"

#	include "ResourceImplement.h"

#	include "PhysicEngine.h"

#	include "LogEngine.h"

#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourcePhysicConvexGeometry );
	//////////////////////////////////////////////////////////////////////////
	ResourcePhysicConvexGeometry::ResourcePhysicConvexGeometry( const ResourceFactoryParam & _params )
		: ResourcePhysicGeometry( _params )
		, m_interface(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePhysicConvexGeometry::loader( XmlElement * _xml )
	{
		ResourcePhysicGeometry::loader( _xml );
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("ConvexGeometryFile"), MENGE_TEXT("Name"), m_filename );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePhysicConvexGeometry::_compile()
	{
		m_interface = Holder<PhysicEngine>::hostage()->cookConvex( m_filename );

		if( m_interface == 0 )
		{
			MENGE_LOG_ERROR( MENGE_TEXT("Warning: resource '%s' can't load convex geometry file '%s'\n")
				, getName().c_str()
				, m_filename.c_str()
				);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePhysicConvexGeometry::_release()
	{
		Holder<PhysicEngine>::hostage()->removeGeometry( m_interface );
	}
	//////////////////////////////////////////////////////////////////////////
	const GeometryInterface * ResourcePhysicConvexGeometry::getGeometry( unsigned int _index ) const
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
}