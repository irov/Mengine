#	pragma once

#	include "ResourceReference.h"
#	include "ResourceTileSet.h"

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
		ResourceTileMap( const std::string & _name );

		unsigned int getWidth() const;
		unsigned int getHeight() const;
		ImageBlock getTile( int _x, int _y );
		float getTileSize();

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		bool _compile() override;
		void _release() override;

	private:
		int m_tileMap[MAX_TILEMAP_WIDTH][MAX_TILEMAP_HEIGHT];
		unsigned int m_width;
		unsigned int m_height;

		std::string m_tileMapFile;
		std::string m_tileSetName;
		ResourceTileSet* m_tileSet;

	};
}	// namespace Menge