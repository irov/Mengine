#	pragma once

#	include "ResourceReference.h"
#	include "Interface/RenderSystemInterface.h"
#	include "Math/vec4.h"
#	include <map>
#	include <vector>

#	define MAX_TILE_NUM 4
#	define MAX_TILESET_WIDTH MAX_TILE_NUM*MAX_TILE_NUM

static int s_tileNumToCode( int tile, int tileNum );

namespace Menge
{

	struct ImageBlock
	{
		mt::vec4f uv;
		RenderImageInterface* image;
	};


	class ResourceTileSet
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceTileSet )

	public:
		ResourceTileSet( const ResourceFactoryParam & _params );

		ImageBlock getImageBlock( int tile );
		float getTileSize();

		void setFilePath( const std::string & _path );

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		std::string m_tileSetFile;
		unsigned int m_tiles;
		//RenderImageInterface* m_image;
		//typedef std::map< int, mt::vec4f > TTileSet;
		typedef std::map< int, RenderImageInterface* > TTileSet;
		TTileSet m_tileSet;
		float m_tileSize;

		typedef std::vector<std::string> TVectorFileNames;
		TVectorFileNames m_vectorFileNames;
		
		//ImageBlock m_tiles[ MAX_TILESET_SIZE ][ MAX_TILESET_SIZE ]
	};
}