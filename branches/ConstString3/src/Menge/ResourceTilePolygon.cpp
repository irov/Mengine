#	include "ResourceTilePolygon.h"

#	include "ResourceImplement.h"

#	include "ResourceManager.h"

#	include "BinParser.h"

#	include "LogEngine.h"

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
	void ResourceTilePolygon::loader( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_NODE( Protocol::Tile )
			{
				TileDecl tileDecl;

				BIN_FOR_EACH_ATTRIBUTES()
				{
					BIN_CASE_ATTRIBUTE( Protocol::Tile_MinAngle, tileDecl.min_angle );
					BIN_CASE_ATTRIBUTE( Protocol::Tile_MaxAngle, tileDecl.max_angle );
					BIN_CASE_ATTRIBUTE( Protocol::Tile_Image, tileDecl.image_resource );
					BIN_CASE_ATTRIBUTE( Protocol::Tile_ImageBack, tileDecl.image_back_resource );
					BIN_CASE_ATTRIBUTE( Protocol::Tile_JuncImage, tileDecl.junc_image_resource );
				}

				m_tiles.push_back( tileDecl );
			}

			BIN_CASE_ATTRIBUTE( Protocol::ImageMap_Name, m_resourcename );
			BIN_CASE_ATTRIBUTE( Protocol::TileJunc_Image, m_juncName );
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
		for(TVectorResourceImage::iterator it = m_imageResources.begin(); it != m_imageResources.end(); it++)
		{
			ResourceImage * resourceImage = (*it);

			resourceImage->decrementReference();
		}

		m_imageResources.clear();

		m_imageJunc->decrementReference();

		m_image->decrementReference();
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

			m_image->decrementReference();

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
