//#	pragma once
//
//#	include "ResourceReference.h"
//#	include "Texture.h"
//#	include "Math/vec4.h"
//#	include <map>
//#	include <vector>
//
//#	define MAX_TILE_NUM 4
//#	define MAX_TILESET_WIDTH MAX_TILE_NUM*MAX_TILE_NUM
//
//namespace Menge
//{
//
//	struct ImageBlock
//	{
//		mt::vec4f uv;
//		Texture* image;
//	};
//
//
//	class ResourceTileSet
//		: public ResourceReference
//	{
//		RESOURCE_DECLARE( ResourceTileSet )
//
//	public:
//		ResourceTileSet();
//
//	public:
//		ImageBlock getImageBlock( int tile );
//		float getTileSize();
//
//		void setFilePath( const String& _path );
//
//	public:
//		void loader( XmlElement * _xml ) override;
//
//	protected:
//		bool _compile() override;
//		void _release() override;
//
//	private:
//		String m_path;
//		size_t m_tiles;
//		//RenderImageInterface* m_image;
//		//typedef std::map< int, mt::vec4f > TTileSet;
//		typedef std::map< int, Texture* > TTileSet;
//		TTileSet m_tileSet;
//		float m_tileSize;
//		
//		//ImageBlock m_tiles[ MAX_TILESET_SIZE ][ MAX_TILESET_SIZE ]
//	};
//}
