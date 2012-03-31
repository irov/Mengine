#	include "Megatextures.h"

namespace Menge
{
	Megatextures::Megatextures( size_t _width, size_t _height, PixelFormat _format )
		: m_width(_width)
		, m_height(_height)
		, m_format(_format)
		, m_enumerator(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterface* Megatextures::createTexture( const ConstString & _name, size_t _width, size_t _height )
	{
		TMapPieces::iterator it_found = m_pieces.find( _name );

		if( it_found != m_pieces.end() )
		{
			const Megatextures::Piece & piece = it_found->second;

			if( piece.dead == false )
			{
				return it_found->second.texture;
			}
			else
			{
				size_t attlasId = piece.attlasId;

				if( attlasId < m_atlases.size() )
				{
					const Megatextures::Atlas & atlas = m_atlases[attlasId];

					size_t x;
					size_t y;
					bool rotate;

					AtlasAllocator * allocator = atlas.allocator;

					if( allocator->causeGhost( piece.id, x, y, rotate ) == true )
					{						
						//TODO
						return 0;
					}
				}
			}
		}

		size_t x;
		size_t y;
		bool rotate;

		size_t atlasId = this->allocateBlock( _width, _height, x, y, rotate );

		if( atlasId == -1 )
		{
			//WTF!!!!
			return 0;
		}

		Megatextures::Piece piece;
		
		piece.texture = 0; //TODO

		size_t new_id = ++m_enumerator;
		piece.id = new_id;
		piece.attlasId = atlasId;

		piece.dead = false;

		m_pieces.insert( std::make_pair(_name, piece) );
				
		return piece.texture;
	}
	//////////////////////////////////////////////////////////////////////////
	size_t Megatextures::allocateBlock( size_t _width, size_t _height, size_t & _x, size_t & _y, bool & _rotate )
	{
		for( TAtlases::iterator
			it = m_atlases.begin(),
			it_end = m_atlases.end();
		it != it_end;
		++it )
		{
			const Megatextures::Atlas & atlas = *it;

			AtlasAllocator * allocator = atlas.allocator;

			if( allocator->allocateBlock( _width, _height, _x, _y, _rotate ) == false )
			{
				continue;
			}
			
			size_t id = std::distance( m_atlases.begin(), it ); 

			return id;
		}

		Megatextures::Atlas atlas;		
		
		atlas.allocator = new AtlasAllocator( m_width, m_height );
		atlas.texture = NULL; //TODO
		
		size_t atlasId = m_atlases.size();

		m_atlases.push_back( atlas );

		AtlasAllocator * allocator = atlas.allocator;

		if( allocator->allocateBlock( _width, _height, _x, _y, _rotate ) == false )
		{
			return -1;
		}

		return atlasId;
	}
}