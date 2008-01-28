#	include "ResourceMesh.h"

#	include "ResourceImplement.h"

#	include "RenderEngine.h"

#	include "LogEngine.h"

#	include "XmlEngine.h"

namespace Menge
{
	RESOURCE_IMPLEMENT( ResourceMesh )
	//////////////////////////////////////////////////////////////////////////
	ResourceMesh::ResourceMesh( const std::string & _name )
		: ResourceReference(_name)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMesh::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "Mesh", "Name", m_meshName );
			XML_CASE_ATTRIBUTE_NODE( "Skeleton", "Name", m_skeletonName );
			XML_CASE_ATTRIBUTE_NODE( "Material", "Name", m_materialName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMesh::_compile()
	{
		m_interface = Holder<RenderEngine>::hostage()->createMesh( m_meshName );

		if( m_interface == 0 )
		{
			MENGE_LOG("Warning: resource '%s' can't load mesh\n"
				, m_meshName.c_str()
			);

			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMesh::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	MeshInterface * ResourceMesh::getMesh() const
	{
		return m_interface;
	}
}