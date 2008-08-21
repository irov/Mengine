#	include "ResourcePhysicConcaveGeometry.h"

#	include "ResourceImplement.h"

#	include "PhysicEngine.h"

#	include "LogEngine.h"

#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourcePhysicConcaveGeometry );
	//////////////////////////////////////////////////////////////////////////
	ResourcePhysicConcaveGeometry::ResourcePhysicConcaveGeometry( const ResourceFactoryParam & _params )
		: ResourcePhysicGeometry( _params )
		, m_interface(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourcePhysicConcaveGeometry::loader( XmlElement * _xml )
	{
		ResourcePhysicGeometry::loader( _xml );
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( MENGE_TEXT("ConcaveGeometryFile"), MENGE_TEXT("Name"), m_filename );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePhysicConcaveGeometry::_compile()
	{
		m_interface = Holder<PhysicEngine>::hostage()->cookConcave( m_params.category + m_filename );

		if( m_interface == 0 )
		{
			MENGE_LOG( MENGE_TEXT("Warning: resource '%s' can't load concave geometry file '%s'\n")
				, getName().c_str()
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
	const GeometryInterface * ResourcePhysicConcaveGeometry::getGeometry( unsigned int _index ) const
	{
		return m_interface;
	}
	//////////////////////////////////////////////////////////////////////////
}