#	include "ResourcePhysicConcaveGeometry.h"

#	include "ResourceImplement.h"

#	include "PhysicEngine.h"

#	include "Logger/Logger.h"

#	include "XmlEngine.h"
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourcePhysicConcaveGeometry );
	//////////////////////////////////////////////////////////////////////////
	ResourcePhysicConcaveGeometry::ResourcePhysicConcaveGeometry()
		: m_interface(0)
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
		const String & category = this->getCategory();

		m_interface = PhysicEngine::hostage()
			->cookConcave( category + m_filename );

		if( m_interface == 0 )
		{
			/*MENGE_LOG_ERROR( "Warning: resource '%s' can't load concave geometry file '%s'\n"
				, Utils::WToA(getName()).c_str()
				, Utils::WToA(m_filename).c_str()
				);*/

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
