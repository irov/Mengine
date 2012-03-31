#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "RenderTexture.h"

#	include "AtlasAllocator/src/AtlasAllocator.h"

#	include <vector>
#	include <map>

namespace Menge
{
	class Megatextures
	{
	public:
		Megatextures( size_t _width, size_t _height, PixelFormat _format );

	public:
		RenderTextureInterface* createTexture( const ConstString & _name, size_t _width, size_t _height );

	protected:
		size_t allocateBlock( size_t _width, size_t _height, size_t & _x, size_t & _y, bool & _rotate );

	protected:
		size_t m_width;
		size_t m_height;

		PixelFormat m_format;

		struct Atlas
		{
			AtlasAllocator * allocator;
			RenderTextureInterface * texture;
		};

		typedef std::vector<Atlas> TAtlases;
		TAtlases m_atlases;

		struct Piece
		{
			RenderTexture * texture;

			size_t id;
			size_t attlasId;
			bool dead;
		};

		typedef std::map<ConstString, Piece> TMapPieces; 
		TMapPieces m_pieces;

		size_t m_enumerator;
	};
}
