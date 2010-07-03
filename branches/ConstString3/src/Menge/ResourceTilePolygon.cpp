#	include "ResourceTilePolygon.h"

#	include "ResourceImplement.h"

#	include "ResourceManager.h"

#	include "XmlEngine.h"

#	include "Logger/Logger.h"

#	include "ResourceImage.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT(ResourceTilePolygon);
	//////////////////////////////////////////////////////////////////////////
	ResourceTilePolygon::ResourceTilePolygon()
		: m_image(0)
		, m_imageJunc(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTilePolygon::loader(XmlElement * _xml)
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "Tile" )
			{
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

			XML_CASE_ATTRIBUTE_NODE( "ImageMap", "Name", m_resourcename );
			XML_CASE_ATTRIBUTE_NODE( "TileJunc", "Image", m_juncName );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	ResourceImage* ResourceTilePolygon::getImage()
	{
		return m_image;
	}
	//////////////////////////////////////////////////////////////////////////
	ResourceImage* ResourceTilePolygon::getPlugImage()
	{
		return m_imageJunc;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTilePolygon::_release()
	{
		for(TResourceVec::iterator it = m_imageResources.begin(); it != m_imageResources.end(); it++)
		{
			ResourceManager::get()
				->releaseResource(*it);
		}

		m_imageResources.clear();

		ResourceManager::get()
			->releaseResource( m_imageJunc );

		ResourceManager::get()
			->releaseResource( m_image );
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceTilePolygon::_compile()
	{
		ResourceManager* resourceManager = ResourceManager::get();

		m_image = resourceManager->getResourceT<ResourceImage>( m_resourcename );

		if( m_image == 0 )
		{
			MENGE_LOG_ERROR( "Image resource not getting '%s'"
				, m_resourcename.c_str() 
				);

			return false;
		}

		m_imageJunc = resourceManager->getResourceT<ResourceImage>( m_juncName );

		if( m_imageJunc == 0 )
		{
			MENGE_LOG_ERROR( "Image resource not getting '%s'"
				, m_juncName.c_str() 
				);

			resourceManager->releaseResource( m_image );

			return false;
		}

		ResourceImage* resourceImage = NULL;

		//TODO: - index -> iterator
		for( TTileDecls::size_type i = 0; i < m_tiles.size(); i++ )
		{
			if( m_tiles[i].image_resource.empty() == false )
			{
				resourceImage = resourceManager->getResourceT<ResourceImage>( m_tiles[i].image_resource );
				if( resourceImage != NULL )
				{
					m_tiles[i].image = resourceImage;
					m_imageResources.push_back( resourceImage );
				}
			}
			if( m_tiles[i].junc_image_resource.empty() == false )
			{
				resourceImage = resourceManager->getResourceT<ResourceImage>( m_tiles[i].junc_image_resource );
				if( resourceImage != NULL )
				{
					m_tiles[i].junc_image = resourceImage;
					m_imageResources.push_back( resourceImage );
				}
			}
			if( m_tiles[i].image_back_resource.empty() == false )
			{
				resourceImage = resourceManager->getResourceT<ResourceImage>( m_tiles[i].image_back_resource );
				if( resourceImage != NULL )
				{
					m_tiles[i].image_back = resourceImage;
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
	const TTileDecls& ResourceTilePolygon::getTileDecls() const
	{
		return m_tiles;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& ResourceTilePolygon::getResourceName() const
	{
		return m_resourcename;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString& ResourceTilePolygon::getJunkName() const
	{
		return m_juncName;
	}
	//////////////////////////////////////////////////////////////////////////
}
