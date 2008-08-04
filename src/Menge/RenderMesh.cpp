#	include "ObjectImplement.h"

#	include "RenderMesh.h"
#	include "XmlEngine.h"
#	include "ResourceMesh.h"
#	include "ResourceManager.h"
#	include "LogEngine.h"
#	include "ResourceMaterial.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT( RenderMesh );
	//////////////////////////////////////////////////////////////////////////
	RenderMesh::RenderMesh()
		: m_resourceMesh( NULL )
		, m_renderTarget( NULL )
		, m_resourceMaterial( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderMesh::~RenderMesh()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh::loader( XmlElement * _xml )
	{
		SceneNode3D::loader( _xml );
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "Resource", "Name", m_resourceName );
			XML_CASE_ATTRIBUTE_NODE( "Material", "Name", m_materialName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh::_render( bool _enableDebug )
	{

		RenderEngine* renderEngine = Holder<RenderEngine>::hostage();
		mt::mat4f wm = renderEngine->getWorldMatrix();

		renderEngine->setWorldMatrix( getLocalMatrix3D() );

		renderEngine->renderMesh( m_resourceMesh->getVertexData(),
			m_resourceMesh->getIndexData(), 
			&m_material 
			);

		renderEngine->setWorldMatrix( wm );
	}
	//////////////////////////////////////////////////////////////////////////
	bool RenderMesh::_compile()
	{
		if( SceneNode3D::_compile() == false )
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
				MENGE_LOG( "Error: Can't find mesh resource '%s'", m_resourceName.c_str() );
				return false;
			}
		}
		else
		{
			return false;
		}

		compileMaterial_();

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh::_release()
	{
		SceneNode3D::_release();

		Holder<ResourceManager>::hostage()
			->releaseResource( m_resourceMesh );

		if( m_renderTarget )
		{
			Holder<RenderEngine>::hostage()
				->releaseImage( m_renderTarget );
		}

		releaseMaterial_();

		m_resourceMesh = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh::createRenderTarget( const String& _name, std::size_t _width, std::size_t _height )
	{
		m_renderTarget = Holder<RenderEngine>::hostage()
			->createRenderTargetImage( _name.c_str(), _width, _height );

		m_material.texture = m_renderTarget;
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh::setMaterial( const String& _materialName )
	{
		if( m_materialName != _materialName )
		{
			m_materialName = _materialName;
			releaseMaterial_();
			compileMaterial_();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh::compileMaterial_()
	{
		if( m_materialName != "" )
		{
			m_resourceMaterial = 
				Holder<ResourceManager>::hostage()
				->getResourceT<ResourceMaterial>( m_materialName );

			if( m_resourceMaterial == 0 )
			{
				MENGE_LOG( "Warning: Can't find material '%s. RenderMesh '%s' will be proceeded whitout material",
					m_materialName.c_str(),
					m_resourceName.c_str()
					);
			}
			else
			{
				m_material = m_resourceMaterial->getMaterial();
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void RenderMesh::releaseMaterial_()
	{
		if( m_resourceMaterial )
		{
			Holder<ResourceManager>::hostage()
				->releaseResource( m_resourceMaterial );
		}
	}
	//////////////////////////////////////////////////////////////////////////
}	// namespace Menge