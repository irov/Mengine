#	include "ResourceTileMap.h"
#	include "ResourceImplement.h"

#	include "FileEngine.h"
#	include "XmlEngine.h"
#	include "LogEngine.h"
#	include "ResourceManager.h"
#	include "Utils.h"
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	RESOURCE_IMPLEMENT( ResourceTileMap );
	//////////////////////////////////////////////////////////////////////////
	ResourceTileMap::ResourceTileMap( const ResourceFactoryParam & _params )
	: ResourceReference( _params )
	, m_width( 0 )
	, m_height( 0 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTileMap::setTileMapPath( const String& _path )
	{
		m_tileMapFile = m_params.category + _path;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTileMap::loader( XmlElement * _xml )
	{
		ResourceReference::loader( _xml );
		XML_SWITCH_NODE(_xml)
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "TileMap", "File", &ResourceTileMap::setTileMapPath );
			XML_CASE_ATTRIBUTE_NODE( "ResourceTileSet", "Name", m_tileSetName );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool ResourceTileMap::_compile()
	{
		//m_physXml = "";
		//m_physXml += "<Node Name = \"" + m_name + "_collision\" Type = \"RigidBody2D\">";
		m_physPos.clear();

		m_tileSet = 
			Holder<ResourceManager>::hostage()
			->getResourceT<ResourceTileSet>( m_tileSetName );

		if( m_tileSet == NULL )
		{
			MENGE_LOG_ERROR( "ResourceTileMap::_compile -> compiling resource failed \"%s\""
				, m_tileSetName.c_str() );
			return false;
		}

		/*char buffer[6];
		float solidSize = m_tileSet->getTileSize() / 2;
		std::string strSolidSize( _itoa( solidSize, buffer, 10 ) );*/
		m_physWidth = m_tileSet->getTileSize();

		m_width = 0;
		m_height = 0;
		DataStreamInterface* mapFile = Holder<FileEngine>::hostage()->openFile( m_tileMapFile );
		String line1 = Utils::getLine( mapFile );
		m_width = line1.size() - 1;
		String line2;

		for( String::size_type i = 0; i < line1.size(); i++ )
		{
			if( line1[i] == '1' )
			{
				//m_physXml += "<ShapeBox><Width Value = \"" + strSolidSize + "\"/><Height Value = \"" + strSolidSize + "\"/><Position Value = \"" +
				//	std::string( _itoa( i*solidSize+ solidSize/2, buffer, 10 ) ) + ";" + std::string( _itoa( solidSize/2, buffer, 10 ) ) + "\"/><Angle Value = \"0\"/></ShapeBox>";
				m_physPos.push_back( mt::vec2f( i * m_physWidth, 0.0f ) );
			}

		}
		while( !mapFile->eof() )
		{		
			line2 = Utils::getLine( mapFile );
			if( m_width != line2.size() - 1 )
			{
				MENGE_LOG_ERROR( "ResourceTileMap::_compile -> Invalid TileMap format in \"%s\""
					, m_tileMapFile.c_str() );
				return false;
			}

			std::vector< int > vect;
			for( unsigned int i = 0; i < m_width; i++ )
			{


				//m_tileMap[i][m_height] = ( line1[i] - 48 ) * 1000 + ( line1[i + 1] - 48 ) * 100 + ( line2[ i + 1] - 48 ) * 10 + ( line2[i] - 48 );
				int t = ( line1[i] - 48 ) * 1000 + ( line1[i + 1] - 48 ) * 100 + ( line2[ i + 1] - 48 ) * 10 + ( line2[i] - 48 );
				vect.push_back( t );

				//
				if( line2[i] == '1' )
				{
				//m_physXml += "<ShapeBox><Width Value = \"" + strSolidSize + "\"/><Height Value = \"" + strSolidSize + "\"/><Position Value = \"" +
				//	std::string( _itoa( i*solidSize + solidSize/2, buffer, 10 ) ) + ";" + std::string( _itoa( (m_height+1)*solidSize + solidSize/2, buffer, 10 ) ) + "\"/><Angle Value = \"0\"/></ShapeBox>";
					m_physPos.push_back( mt::vec2f( i * m_physWidth, (m_height+1)*m_physWidth ) );
				}
			}
			if( line2[m_width] == '1' )
			{
				//m_physXml += "<ShapeBox><Width Value = \"" + strSolidSize + "\"/><Height Value = \"" + strSolidSize + "\"/><Position Value = \"" +
				//	std::string( _itoa( m_width*solidSize + solidSize/2, buffer, 10 ) ) + ";" + std::string( _itoa( (m_height+1)*solidSize + solidSize/2, buffer, 10 ) ) + "\"/><Angle Value = \"0\"/></ShapeBox>";
				m_physPos.push_back( mt::vec2f( m_width * m_physWidth, (m_height+1)*m_physWidth ) );
			}
			m_tileMap.push_back( vect );
			m_height++;
			line1 = line2;
		}

		Holder<FileEngine>::hostage()->closeStream( mapFile );

//		m_physXml += "<Density Value = \"0.0\"/>";

	//	m_physXml += "</Node>";

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void ResourceTileMap::_release()
	{
		
	}
	//////////////////////////////////////////////////////////////////////////
	ImageBlock ResourceTileMap::getTile( std::size_t _x, std::size_t _y )
	{
		return m_tileSet->getImageBlock( m_tileMap[ _y ][ _x ] );
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceTileMap::getWidth() const
	{
		return m_width;
	}
	//////////////////////////////////////////////////////////////////////////
	std::size_t ResourceTileMap::getHeight() const
	{
		return m_height;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceTileMap::getTileSize()
	{
		return m_tileSet->getTileSize();
	}
	//////////////////////////////////////////////////////////////////////////
	/*const std::string& ResourceTileMap::getPhysXml() const
	{
		return m_physXml;
	}*/
	//////////////////////////////////////////////////////////////////////////
	const std::vector< mt::vec2f >& ResourceTileMap::_getPhysPos() const
	{
		return m_physPos;
	}
	//////////////////////////////////////////////////////////////////////////
	float ResourceTileMap::_getPhysWidth() const
	{
		return m_physWidth;
	}
	//////////////////////////////////////////////////////////////////////////
}	// namescape Menge
