#	include "ResourceTileMap.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"
#	include "XmlEngine.h"
#	include "LogEngine.h"
#	include "ResourceManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceTileMap )
	//////////////////////////////////////////////////////////////////////////
	
	//////////////////////////////////////////////////////////////////////////
	ResourceTileMap::ResourceTileMap( const std::string & _name )
	: ResourceReference( _name )
	, m_width( 0 )
	, m_height( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTileMap::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "TileMap", "File", m_tileMapFile );
			XML_CASE_ATTRIBUTE_NODE( "ResourceTileSet", "Name", m_tileSetName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceTileMap::_compile()
	{
		m_width = 0;
		m_height = 0;
		FileDataInterface* mapFile = Holder<FileEngine>::hostage()->openFile( m_tileMapFile );
		std::string line1 = mapFile->getLine( true );
		m_width = line1.size() - 1;
		std::string line2;
		/*std::string line2 = mapFile->getLine( true );
		if( m_width != line2.size() - 1)
		{
			MENGE_LOG( "ResourceTileMap::_compile -> Invalid TileMap format in %s", m_tileMapFile );
			return false;
		}
		for( int i = 0; i < m_width; i++ )
		{
			int a = line1[2*i] - 48;
			m_tileMap[i][m_height] = ( line1[2*i] - 48 ) * 1000 + ( line1[2*i + 1] - 48 ) * 100 + ( line2[ 2*i + 1] - 48 ) * 10 + ( line2[2*i] - 48 );
		}*/
		while( !mapFile->eof() )
		{		
			//line1 = mapFile->getLine( true );
			line2 = mapFile->getLine( true );
			if( m_width != line2.size() - 1 )
			{
				MENGE_LOG( "ResourceTileMap::_compile -> Invalid TileMap format in %s", m_tileMapFile );
				return false;
			}
			for( int i = 0; i < m_width; i++ )
			{
				m_tileMap[i][m_height] = ( line1[i] - 48 ) * 1000 + ( line1[i + 1] - 48 ) * 100 + ( line2[ i + 1] - 48 ) * 10 + ( line2[i] - 48 );
			}
			m_height++;
			line1 = line2;
		}
		//m_height++;
		Holder<FileEngine>::hostage()->closeFile( mapFile );

		m_tileSet = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceTileSet>( m_tileSetName );

		if( m_tileSet == NULL )
		{
			MENGE_LOG( "ResourceTileMap::_compile -> compiling resource %s failed", m_tileSetName.c_str() );
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTileMap::_release()
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	ImageBlock ResourceTileMap::getTile( int _x, int _y )
	{
		return m_tileSet->getImageBlock( m_tileMap[ _x ][ _y ] );
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ResourceTileMap::getWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ResourceTileMap::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ResourceTileMap::getTileSize()
	{
		return m_tileSet->getTileSize();
	}
	//////////////////////////////////////////////////////////////////////////
}	// namescape Menge