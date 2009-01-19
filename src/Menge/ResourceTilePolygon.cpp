#	include "ResourceTilePolygon.h"

#	include "ResourceImplement.h"

#	include "XmlEngine.h"

#	include "LogEngine.h"

#	include "math/triangulation.h"

#	include "ResourceImage.h"

#	include "Interface/RenderSystemInterface.h"

#	include "ResourceManager.h"

#	include "math/angle.h"
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT(ResourceTilePolygon);
	//////////////////////////////////////////////////////////////////////////
	ResourceTilePolygon::ResourceTilePolygon(const ResourceFactoryParam & _params)
		: ResourceReference(_params)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTilePolygon::loader(XmlElement * _xml)
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Tile" )
			{
				//float minAngle;
				//float maxAngle;
				//String imageName;
				TileDecl tileDecl;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "MinAngle", tileDecl.min_angle );
					XML_CASE_ATTRIBUTE( "MaxAngle", tileDecl.max_angle );
					XML_CASE_ATTRIBUTE( "Image", tileDecl.image_resource );
					XML_CASE_ATTRIBUTE( "ImageBack", tileDecl.image_back_resource );
					XML_CASE_ATTRIBUTE( "JuncImage", tileDecl.junc_image_resource );
				}

				m_tiles.push_back( tileDecl );
			}

			XML_CASE_NODE( "TileJunc" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Image", m_juncName );
				}
			}

			XML_CASE_ATTRIBUTE_NODE( "ImageMap", "Name", m_resourcename );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceTilePolygon::getImage() const
	{
		return m_image;
	}
	//////////////////////////////////////////////////////////////////////////
	const RenderImageInterface * ResourceTilePolygon::getPlugImage() const
	{
		return m_imageJunc;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTilePolygon::_release()
	{
		for(std::vector<ResourceImage*>::iterator it = m_imageResources.begin(); it != m_imageResources.end(); it++)
		{
			Holder<ResourceManager>::hostage()->releaseResource(*it);
		}

		m_imageResources.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceTilePolygon::_compile()
	{
		ResourceManager* resourceManager = Holder<ResourceManager>::hostage();

		ResourceImage* resourceImage = 
			resourceManager->getResourceT<ResourceImage>( m_resourcename );

		if( resourceImage == 0 )
		{
			MENGE_LOG_ERROR( "Image resource not getting \"%s\""
				, m_resourcename.c_str() );
			return false;
		}

		m_image = resourceImage->getImage( 0 );
		m_imageResources.push_back( resourceImage );

		resourceImage = 
			resourceManager->getResourceT<ResourceImage>( m_juncName );

		if( resourceImage == 0 )
		{
			MENGE_LOG_ERROR( "Image resource not getting \"%s\""
				, m_juncName.c_str() );
			return false;
		}

		m_imageJunc = resourceImage->getImage( 0 );
		m_imageResources.push_back( resourceImage );

		for( TTileDecls::size_type i = 0; i < m_tiles.size(); i++ )
		{
			if( m_tiles[i].image_resource.empty() == false )
			{
				resourceImage = resourceManager->getResourceT<ResourceImage>( m_tiles[i].image_resource );
				if( resourceImage != NULL )
				{
					m_tiles[i].image = resourceImage->getImage( 0 );
					m_imageResources.push_back( resourceImage );
				}
			}
			if( m_tiles[i].junc_image_resource.empty() == false )
			{
				resourceImage = resourceManager->getResourceT<ResourceImage>( m_tiles[i].junc_image_resource );
				if( resourceImage != NULL )
				{
					m_tiles[i].junc_image = resourceImage->getImage( 0 );
					m_imageResources.push_back( resourceImage );
				}
			}
			if( m_tiles[i].image_back_resource.empty() == false )
			{
				resourceImage = resourceManager->getResourceT<ResourceImage>( m_tiles[i].image_back_resource );
				if( resourceImage != NULL )
				{
					m_tiles[i].image_back = resourceImage->getImage( 0 );
					m_imageResources.push_back( resourceImage );
				}
			}
			else
			{
				m_tiles[i].image_back = NULL;
			}
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const ResourceTilePolygon::TTileDecls& ResourceTilePolygon::getTileDecls() const
	{
		return m_tiles;
	}
	//////////////////////////////////////////////////////////////////////////
}
