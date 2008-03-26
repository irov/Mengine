#	include "ResourceTileSet.h"
#	include "ResourceImplement.h"
#	include "XmlEngine.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"

static int s_tileNumToCode( int tile, int tileNum )
{
	int ret = 0;
	int d = 1;
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
	RESOURCE_IMPLEMENT( ResourceTileSet )
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	ResourceTileSet::ResourceTileSet( const std::string & _name )
		: ResourceReference( _name )
		, m_tileSize( 0 )
		, m_tiles( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTileSet::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE( "TileSet", "File", m_tileSetFile );
			XML_CASE_ATTRIBUTE_NODE( "Tiles", "Value", m_tiles );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceTileSet::_compile()
	{
		m_image = Holder<RenderEngine>::hostage()->loadImage( m_tileSetFile, 0 );
		
		if( m_image == 0 )
		{
			MENGE_LOG("Error: resource '%s' can't load image file '%s'\n"
				, m_name.c_str()
				, m_tileSetFile.c_str()
				);

			return false;
		}

		unsigned int tileSetSize = m_tiles * m_tiles;
		m_tileSize = m_image->getWidth() / tileSetSize;
		int tilesNum = tileSetSize * tileSetSize;

		for( int tile = 0; tile < tilesNum; tile++ )
		{
			int tileCode = s_tileNumToCode( tile, m_tiles );
			int y = tile / tileSetSize;
			int x = tile % tileSetSize;
			mt::vec4f uv( x * m_tileSize, y * m_tileSize, ( x + 1 ) * m_tileSize, ( y + 1 ) * m_tileSize );
			uv /= m_image->getWidth();
			m_tileSet[ tileCode ] = uv;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTileSet::_release()
	{
		Holder<RenderEngine>::hostage()->releaseImage( m_image );
	}
	//////////////////////////////////////////////////////////////////////////
	ImageBlock ResourceTileSet::getImageBlock( int tile )
	{
		ImageBlock block;
		block.image = m_image;
		block.uv = m_tileSet[ tile ];
		return block;
	}
	//////////////////////////////////////////////////////////////////////////
	unsigned int ResourceTileSet::getTileSize()
	{
		return m_tileSize;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namescape Menge