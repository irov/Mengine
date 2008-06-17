#	include "ResourceMesh.h"

#	include "ResourceImplement.h"

#	include "RenderEngine.h"

#	include "LogEngine.h"

#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceMesh );
	//////////////////////////////////////////////////////////////////////////
	ResourceMesh::ResourceMesh( const ResourceFactoryParam & _params )
		: ResourceReference( _params )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMesh::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "File", "Path", m_filename );
			XML_CASE_ATTRIBUTE_NODE( "Mesh", "Name", m_meshName );
			//XML_CASE_ATTRIBUTE_NODE( "Skeleton", "Name", m_skeletonName );
			//XML_CASE_ATTRIBUTE_NODE( "Material", "Name", m_materialName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const std::vector<TVertex>& ResourceMesh::getVertexData()
	{
		assert( 0 && "Object of this class must never exist" );
		return std::vector<TVertex>();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceMesh::_compile()
	{
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceMesh::_release()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & ResourceMesh::getMeshName() const
	{
		return m_meshName;
	}
	//////////////////////////////////////////////////////////////////////////
	/*const std::string & ResourceMesh::getSkeletonName() const
	{
		return m_skeletonName;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & ResourceMesh::getMaterialName() const
	{
		return m_materialName;
	}*/
	//////////////////////////////////////////////////////////////////////////
}