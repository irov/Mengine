#	include "ObjectImplement.h"

#	include "RenderMesh.h"
#	include "XmlEngine.h"
#	include "ResourceMesh.h"
#	include "ResourceManager.h"
#	include "LogEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( RenderMesh );
	//////////////////////////////////////////////////////////////////////////
	RenderMesh::RenderMesh()
		: m_resourceMesh( NULL )
	{
		/*TVertex v;
		for( int i=0; i<50; i++ )
		{
			float theta = (2*mt::m_pi*i)/(50-1);
			
			v.x = sinf(theta); v.y = -1.0f; v.z = cosf(theta);
			v.nx = sinf(theta); v.ny = 0.0f; v.nz = cosf(theta);
			m_vertexData.push_back( v );
			v.x = sinf(theta); v.y = 1.0f; v.z = cosf(theta);
			v.nx = sinf(theta); v.ny = 0.0f; v.nz = cosf(theta);
			m_vertexData.push_back( v );
		}*/

	}
	//////////////////////////////////////////////////////////////////////////
	RenderMesh::~RenderMesh()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh::loader( XmlElement * _xml )
	{
		SceneNode3D_::loader( _xml );
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "Resource", "Name", m_resourceName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh::_render()
	{
		Holder<RenderEngine>::hostage()
			->renderMesh( m_resourceMesh->getVertexData(), 0 );
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderMesh::_compile()
	{
		if( SceneNode3D_::_compile() == false )
		{
			return false;
		}

		if( m_resourceName != "" )
		{
			m_resourceMesh = 
				Holder<ResourceManager>::hostage()
				->getResourceT<ResourceMesh>( m_resourceName );

			if( m_resourceMesh == 0 )
			{
				MENGE_LOG( "Image resource not getting '%s'", m_resourceName.c_str() );
				return false;
			}
		}
		else
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh::_release()
	{
		SceneNode3D_::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceMesh );

		m_resourceMesh = 0;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge