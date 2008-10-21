#	include "ResourceTileSet.h"
#	include "ResourceImplement.h"
#	include "XmlEngine.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"
#	include "Utils.h"

static unsigned int s_tileNumToCode( unsigned int tile, unsigned int tileNum )
{
	unsigned int ret = 0;
	unsigned int d = 1;
	while( tile )
	{
		ret += ( tile % tileNum ) * d;
		tile /= tileNum;
		d *= 10;
	}
	return ret;
}

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceTileSet );
	//////////////////////////////////////////////////////////////////////////
	ResourceTileSet::ResourceTileSet( const ResourceFactoryParam & _params )
		: ResourceReference( _params )
		, m_tileSize( 0 )
		, m_tiles( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTileSet::setFilePath( const String& _path )
	{
		m_tileSetFile = m_params.category + _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTileSet::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "TileSet", "File", &ResourceTileSet::setFilePath );
			XML_CASE_ATTRIBUTE_NODE( "Tiles", "Value", m_tiles );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceTileSet::_compile()
	{
		/*m_image = Holder<RenderEngine>::hostage()->loadImage( m_tileSetFile, 0 );
		
		if( m_image == 0 )
		{
			MENGE_LOG("Error: resource '%s' can't load image file '%s'\n"
				, getName().c_str()
				, m_tileSetFile.c_str()
				);

			return false;
		}

		unsigned int tileSetSize = m_tiles * m_tiles;
		m_tileSize = m_image->getWidth() / tileSetSize - 1;
		unsigned int tilesNum = tileSetSize * tileSetSize;
		mt::vec4f uv( 0.0f, 0.0f, 1.0f, 1.0f );
		for( unsigned int tile = 0; tile < tilesNum; tile++ )
		{
			unsigned int tileCode = s_tileNumToCode( tile, m_tiles );
			
			unsigned int y = tile / tileSetSize;
			unsigned int x = tile % tileSetSize;
			mt::vec4f uv( x * ( m_tileSize + 0.5f ), y * ( m_tileSize + 0.5f ), ( x + 1 ) * ( m_tileSize + 1 ), ( y + 1 ) * ( m_tileSize + 1 ) );
			uv /= m_image->getWidth();
			m_tileSet[ tileCode ] = uv;
		}*/
		unsigned int tileSetSize = m_tiles * m_tiles;
		unsigned int tilesNum = tileSetSize * tileSetSize;
	
		for( unsigned int tile = 0; tile < tilesNum; tile++ )
		{
			std::size_t tileCode = s_tileNumToCode( tile, m_tiles );
			TChar buffer[10];
			//String tilestr( std::itoa(tileCode, buffer, 10) );
			String tilestr = Utils::toString( tileCode );
			String name = m_tileSetFile + tilestr + ".png";
			RenderImageInterface* image = Holder<RenderEngine>::hostage()->loadImage( name, 0 );

			m_tileSize = (float)image->getWidth();
			m_tileSet[ tileCode ] = image;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTileSet::_release()
	{
		for( TTileSet::iterator
			it = m_tileSet.begin(),
			it_end = m_tileSet.end();
		it != it_end;
		++it)
		{
			Holder<RenderEngine>::hostage()->releaseImage( (*it).second );
		}
		m_tileSet.clear();
		//Holder<RenderEngine>::hostage()->releaseImage( m_image );
	}
	//////////////////////////////////////////////////////////////////////////
	ImageBlock ResourceTileSet::getImageBlock( int tile )
	{
		ImageBlock block;
		//block.image = m_image;
		//block.uv = m_tileSet[ tile ];
		block.image = m_tileSet[ tile ];
		block.uv = mt::vec4f( 0.0f, 0.0f, 1.0f, 1.0f );
		return block;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceTileSet::getTileSize()
	{
		return m_tileSize;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namescape Menge