#	include "ResourcePhysicConvexGeometry.h"

#	include "ResourceImplement.h"

#	include "PhysicEngine.h"

#	include "LogEngine.h"

#	include "XmlEngine.h"
#	include "Utils.h"
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
			XML_CASE_ATTRIBUTE_NODE( "ConvexGeometryFile", "Name", m_filename );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourcePhysicConvexGeometry::_compile()
	{
		m_interface = Holder<PhysicEngine>::hostage()->cookConvex( m_filename );

		if( m_interface == 0 )
		{
			MENGE_LOG_ERROR << "Warning: resource \"" << getName() << "\" can't load convex geometry file " << m_filename;
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