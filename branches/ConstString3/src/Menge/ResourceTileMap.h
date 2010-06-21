#	pragma once

#	include "ResourceReference.h"
#	include "ResourceTileSet.h"
#	include <vector>
#	define MAX_TILEMAP_WIDTH 1024
#	define MAX_TILEMAP_HEIGHT 1024

namespace Menge
{
	class ResourceTileSet;

	/*struct ImageBlock
	{
		mt::vec4f uv;
		RenderImageInterface * image;
	};*/

	typedef std::vector<mt::vec2f> TileMapPhysicPosition;

	class ResourceTileMap
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceTileMap )

	public:
		ResourceTileMap();

	public:
		std::size_t getWidth() const;
		std::size_t getHeight() const;
		ImageBlock getTile( std::size_t _x, std::size_t _y );
		float getTileSize();
		//const std::string& getPhysXml() const;

		void setTileMapPath( const String& _path );

	public:
		void loader( XmlElement * _xml ) override;
		const TileMapPhysicPosition & _getPhysPos() const;
		float _getPhysWidth() const;

	protected:
		bool _compile() override;
		void _release() override;

	private:

		typedef std::vector< std::vector< int > > TMapTiles;
		TMapTiles m_tiles;
		//int m_tileMap[MAX_TILEMAP_WIDTH][MAX_TILEMAP_HEIGHT];
		std::size_t m_width;
		std::size_t m_height;

		String m_tileMapFile;
		ConstString m_tileSetName;
		ResourceTileSet* m_tileSet;
		float m_physWidth;
		TileMapPhysicPosition m_physPos;
		//std::string m_physXml;

	};
}	// namespace Menge
