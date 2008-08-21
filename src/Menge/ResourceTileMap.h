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

	class ResourceTileMap
		: public ResourceReference
	{
		RESOURCE_DECLARE( ResourceTileMap )

	public:
		ResourceTileMap( const ResourceFactoryParam & _params );

		std::size_t getWidth() const;
		std::size_t getHeight() const;
		ImageBlock getTile( std::size_t _x, std::size_t _y );
		float getTileSize();
		//const std::string& getPhysXml() const;

		void setTileMapPath( const String& _path );

	public:
		void loader( XmlElement * _xml ) override;
		const std::vector< mt::vec2f >& _getPhysPos() const;
		float _getPhysWidth() const;

	protected:
		bool _compile() override;
		void _release() override;

	private:

		std::vector< std::vector< int > > m_tileMap;
		//int m_tileMap[MAX_TILEMAP_WIDTH][MAX_TILEMAP_HEIGHT];
		std::size_t m_width;
		std::size_t m_height;

		String m_tileMapFile;
		String m_tileSetName;
		ResourceTileSet* m_tileSet;
		float m_physWidth;
		std::vector< mt::vec2f > m_physPos;
		//std::string m_physXml;

	};
}	// namespace Menge