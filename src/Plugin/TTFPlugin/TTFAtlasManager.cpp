#	include "TTFAtlasManager.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TTFAtlasManager::TTFAtlasManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TTFAtlasManager::~TTFAtlasManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterfacePtr TTFAtlasManager::makeTextureGlyph( uint32_t _height, uint32_t _width, const void * _buffer, uint32_t _pitch )
	{
		uint32_t width = Helper::getTexturePOW2( _width );
		uint32_t height = Helper::getTexturePOW2( _height );

		uint32_t dimension = std::max( _width, _height );

		uint32_t atlas_index = dimension >> MENGINE_TTF_MIN_ATLAS_POW;

		if( atlas_index > MENGINE_TTF_MAX_ATLAS_POW )
		{
			return nullptr;
		}

		TVectorAtlases & atlases = m_atlasess[atlas_index];

		for( TVectorAtlases::iterator
			it = atlases.begin(),
			it_end = atlases.end();
		it != it_end;
		++it )
		{
			const Atlas & atlas = *it;
		}
	}
}