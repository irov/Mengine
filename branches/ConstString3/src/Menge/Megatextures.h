#	pragma once

#	include "Interface/RenderSystemInterface.h"

#	include "RenderTexture.h"

#	include "AtlasAllocator/src/AtlasAllocator.h"

#	include <vector>
#	include <map>

namespace Menge
{
	class Megatextures
		: public RenderTextureInterfaceListener
	{
	public:
		Megatextures( size_t _width, size_t _height, PixelFormat _format );

	public:
		PixelFormat getPixelFormat() const;
		size_t getMegatextureCount() const;

	public:
		RenderTextureInterface* createTexture( size_t _width, size_t _height );

	protected:
		void onRenderTextureRelease( const RenderTextureInterface * _texture ) override;

	protected:
		size_t m_width;
		size_t m_height;

		PixelFormat m_pixelFormat;

		struct Atlas
		{
			AtlasAllocator * allocator;
			RenderTextureInterface * texture;
		};

		typedef std::list<Atlas> TAtlases;
		TAtlases m_atlases;

		struct Piece
		{
			RenderTextureInterface * texture;

			size_t id;

			TAtlases::iterator atlas;
			TPacketBlocks::iterator block;

			bool dead;
		};

		typedef std::map<const RenderTextureInterface *, Piece> TMapPieces; 
		TMapPieces m_pieces;

		size_t m_enumerator;

	private:
		TAtlases::iterator allocateBlock_( size_t _width, size_t _height, size_t & _x, size_t & _y, bool & _rotate );
		TAtlases::iterator newAtlas_();
	};
}
