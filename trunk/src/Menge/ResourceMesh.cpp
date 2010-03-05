#	include "ResourceMesh.h"

#	include "ResourceImplement.h"

#	include "RenderEngine.h"

#	include "Logger/Logger.h"

#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceMesh );
	//////////////////////////////////////////////////////////////////////////
	ResourceMesh::ResourceMesh()
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
	const std::vector<Vertex3D>& ResourceMesh::getVertexData()
	{
		assert( 0 && "Object of this class must never exist" );
		static std::vector<Vertex3D> s_empty;
		return s_empty;
	}
	//////////////////////////////////////////////////////////////////////////
	const std::vector<uint16>& ResourceMesh::getIndexData()
	{
		assert( 0 && "Object of this class must never exist" );
		static std::vector<uint16> s_empty;
		return s_empty;
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
	const String& ResourceMesh::getMeshName() const
	{
		return m_meshName;
	}
	//////////////////////////////////////////////////////////////////////////
	const String& ResourceMesh::getFileName() const
	{
		return m_filename;
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
