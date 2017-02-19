#	include "TTFAtlasService.h"

#	include "Interface/ConfigInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( TTFAtlasService, Menge::TTFAtlasService );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TTFAtlasService::TTFAtlasService()
		: m_minAtlasPow(0)
		, m_maxAtlasPow(0)
		, m_maxAtlasWidth(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TTFAtlasService::~TTFAtlasService()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool TTFAtlasService::_initialize()
	{
		m_minAtlasPow = CONFIG_VALUE( m_serviceProvider, "TTF", "MinAtlasPow", 4U );
		m_maxAtlasPow = CONFIG_VALUE( m_serviceProvider, "TTF", "MaxAtlasPow", 8U );
		m_maxAtlasWidth = CONFIG_VALUE( m_serviceProvider, "TTF", "MaxAtlasWidth", 2048U );

		uint32_t mixAtlasPow = m_maxAtlasPow - m_minAtlasPow;

		m_atlasess.resize( mixAtlasPow );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TTFAtlasService::_finalize()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterfacePtr TTFAtlasService::makeTextureGlyph( uint32_t _width, uint32_t _height, const void * _buffer, uint32_t _pitch, mt::uv4f & _uv )
	{
		uint32_t width = Helper::getTexturePOW2( _width );
		uint32_t height = Helper::getTexturePOW2( _height );

		uint32_t dimension = std::max( _width, _height );

		TTFAtlas * atlas = this->getAtlas_( dimension );

		uint32_t index = atlas->indices.back();
		atlas->indices.pop_back();

		const RenderTextureInterfacePtr & texture = atlas->texture;

		Rect r;
		r.left = index * dimension;
		r.top = 0;
		r.right = r.left + dimension;
		r.bottom = dimension;

		size_t pitch;
		void * memory = texture->lock( &pitch, 0, r, false );

		texture->unlock( 0 );

		float atlas_width_inv = 1.f / float( m_maxAtlasWidth );
		float atlas_height_inv = 1.f / float( dimension );

		_uv.p0 = mt::vec2f( float( r.left ) * atlas_width_inv, float( r.top ) * atlas_height_inv );
		_uv.p1 = mt::vec2f( float( r.right ) * atlas_width_inv, float( r.top ) * atlas_height_inv );
		_uv.p2 = mt::vec2f( float( r.right ) * atlas_width_inv, float( r.bottom ) * atlas_height_inv );
		_uv.p3 = mt::vec2f( float( r.left ) * atlas_width_inv, float( r.bottom ) * atlas_height_inv );

		return texture;
	}
	//////////////////////////////////////////////////////////////////////////
	TTFAtlas * TTFAtlasService::getAtlas_( uint32_t _dimension )
	{
		uint32_t atlas_index = _dimension >> m_minAtlasPow;

		if( atlas_index > m_maxAtlasPow )
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
			TTFAtlas & atlas = *it;

			if( atlas.indices.empty() == true )
			{
				continue;
			}
			
			return &atlas;
		}

		TTFAtlas new_atlas;

		new_atlas.dimension = _dimension;

#   ifdef MENGE_RENDER_TEXTURE_RGBA
        PixelFormat format = PF_A8B8G8R8;
#	else
        PixelFormat format = PF_A8R8G8B8;        
#	endif

		RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE( m_serviceProvider )
			->createDynamicTexture( m_maxAtlasWidth, _dimension, 4, 0, format );

		new_atlas.texture = texture;

		uint32_t glyph_count = m_maxAtlasWidth / _dimension;

		new_atlas.indices.reserve( glyph_count );

		for( uint32_t i = 0; i != glyph_count; ++i )
		{
			new_atlas.indices.push_back( i );
		}

		atlases.push_back( new_atlas );
		
		TTFAtlas & atlas = atlases.back();

		return &atlas;
	}
}