//#	include "Megatextures.h"
//
//#	include "RenderEngine.h"
//
//namespace Menge
//{
//	Megatextures::Megatextures( size_t _width, size_t _height, PixelFormat _format )
//		: m_width(_width)
//		, m_height(_height)
//		, m_pixelFormat(_format)
//		, m_enumerator(0)
//	{
//	}
//	//////////////////////////////////////////////////////////////////////////
//	PixelFormat Megatextures::getPixelFormat() const
//	{
//		return m_pixelFormat;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	size_t Megatextures::getMegatextureCount() const
//	{
//		return m_atlases.size();
//	}
//	//////////////////////////////////////////////////////////////////////////
//	RenderTextureInterface * Megatextures::createTexture( size_t _width, size_t _height )
//	{
//		size_t x;
//		size_t y;
//		bool rotate;
//
//		TAtlases::iterator it_atlas = this->allocateBlock_( _width, _height, x, y, rotate );
//
//		if( it_atlas == m_atlases.end() )
//		{
//			//WTF!!!!
//			return 0;
//		}
//		
//		Megatextures::Piece piece;
//
//		piece.dead = false;
//		piece.atlas = it_atlas;
//
//		size_t new_id = ++m_enumerator;
//		piece.id = new_id;
//
//		const Megatextures::Atlas & atlas = *piece.atlas;
//		AtlasAllocator * allocator = atlas.allocator;
//
//		TPacketBlocks::iterator it_block = allocator->createBlock( piece.id, x, y, _width, _height, rotate );
//		piece.block = it_block;
//
//		RenderTextureInterface * atlasTexture = piece.atlas->texture;
//
//		Rect rect;
//		rect.left = x;
//		rect.top = y;
//		rect.right = x + _width;
//		rect.bottom = y + _height;
//
//		if( rect.right > m_width || rect.bottom > m_height )
//		{
//			return NULL;
//		}
//
//		piece.texture = RenderEngine::get()
//			->createSubTexture( atlasTexture, rect, this ); //TODO
//		
//		m_pieces.insert( std::make_pair(piece.texture, piece) );
//				
//		return piece.texture;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	void Megatextures::onRenderTextureRelease( const RenderTextureInterface * _texture )
//	{
//		TMapPieces::iterator it_found = m_pieces.find( _texture );
//
//		if( it_found == m_pieces.end() )
//		{			
//			return;
//		}
//
//		Piece & piece = it_found->second;
//
//		Atlas & atlas = *(piece.atlas);
//
//		atlas.allocator->removeBlock( piece.block );
//
//		piece.dead = true;
//
//		m_pieces.erase( it_found );
//	}
//	//////////////////////////////////////////////////////////////////////////
//	Megatextures::TAtlases::iterator Megatextures::allocateBlock_( size_t _width, size_t _height, size_t & _x, size_t & _y, bool & _rotate )
//	{
//		for( TAtlases::iterator
//			it = m_atlases.begin(),
//			it_end = m_atlases.end();
//		it != it_end;
//		++it )
//		{
//			const Megatextures::Atlas & atlas = *it;
//
//			AtlasAllocator * allocator = atlas.allocator;
//
//			if( allocator->allocateBlock( _width, _height, _x, _y, _rotate ) == false )
//			{
//				continue;
//			}
//			
//			return it;
//		}
//
//		TAtlases::iterator it_atlas = this->newAtlas_();
//
//		Atlas & atlas = *it_atlas;
//		
//		AtlasAllocator * allocator = atlas.allocator;
//
//		if( allocator->allocateBlock( _width, _height, _x, _y, _rotate ) == false )
//		{
//			return m_atlases.end();
//		}
//
//		return it_atlas;
//	}
//	//////////////////////////////////////////////////////////////////////////
//	Megatextures::TAtlases::iterator Megatextures::newAtlas_()
//	{
//		Megatextures::Atlas atlas;		
//
//		atlas.allocator = new AtlasAllocator( m_width, m_height );
//
//		atlas.texture = RenderEngine::get()
//			->createTexture( m_width, m_height, m_pixelFormat );
//
//		size_t atlasId = m_atlases.size();
//
//		TAtlases::iterator it_insert = m_atlases.insert( m_atlases.end(), atlas );
//
//		return it_insert;
//	}
//}