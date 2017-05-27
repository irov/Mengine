#	include "TTFAtlasService.h"

#	include "Interface/ConfigInterface.h"

#   include "stdex/memorycopy.h"

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

        for( uint32_t i = 0; i != 2; ++i )
        {
            m_atlasess[i].resize( mixAtlasPow );
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TTFAtlasService::_finalize()
	{
		for( uint32_t i = 0; i != 2; ++i )
		{
			m_atlasess[i].clear();
		}
	}
	//////////////////////////////////////////////////////////////////////////
	RenderTextureInterfacePtr TTFAtlasService::makeTextureGlyph( uint32_t _width, uint32_t _height, uint32_t _channel, const void * _buffer, uint32_t _pitch, mt::uv4f & _uv )
	{
		uint32_t hw_width = Helper::getTexturePOW2( _width );
		uint32_t hw_height = Helper::getTexturePOW2( _height );

		uint32_t dimension = std::max( hw_width, hw_height );

		if( dimension == 0 )
		{
			return nullptr;
		}

		TTFAtlas * atlas = this->getAtlas_( dimension, _channel );

		uint32_t index = atlas->indices.back();
		atlas->indices.pop_back();

		const RenderTextureInterfacePtr & texture = atlas->texture;

		Rect r;
		r.left = index * dimension;
		r.top = 0;
		r.right = r.left + dimension;
		r.bottom = dimension;

        const RenderImageInterfacePtr & texture_image = texture->getImage();

        uint32_t texture_channel = texture_image->getHWChannels();

		const RenderImageInterfacePtr & image = texture->getImage();

        size_t texture_pitch;
		uint8_t * texture_memory = image->lock( &texture_pitch, 0, r, false );

        const uint8_t * glyph_buffer = reinterpret_cast<const uint8_t *>(_buffer);

        uint8_t * it_texture_memory = texture_memory;
        const uint8_t * it_glyph_buffer = glyph_buffer;
        
        if( _channel == 1 && texture_channel == 4 )
        {
            for( uint32_t h = 0; h != _height; ++h )
            {
                for( uint32_t w = 0; w != _width; ++w )
                {
                    *(it_texture_memory + w * 4 + 0) = 255;
                    *(it_texture_memory + w * 4 + 1) = 255;
                    *(it_texture_memory + w * 4 + 2) = 255;
                    *(it_texture_memory + w * 4 + 3) = *(it_glyph_buffer + w);
                }

                it_texture_memory += texture_pitch;
                it_glyph_buffer += _pitch;
            }
        }
        else
        {
            for( uint32_t h = 0; h != _height; ++h )
            {
                stdex::memorycopy_pod( it_texture_memory, 0, it_glyph_buffer, _width * _channel );

                it_texture_memory += texture_pitch;
                it_glyph_buffer += _pitch;
            }
        }

		image->unlock( 0, true );

		float atlas_width_inv = 1.f / float( m_maxAtlasWidth );
		float atlas_height_inv = 1.f / float( dimension );

		_uv.p0 = mt::vec2f( float( r.left ) * atlas_width_inv, float( r.top ) * atlas_height_inv );
		_uv.p1 = mt::vec2f( float( r.left + _width ) * atlas_width_inv, float( r.top ) * atlas_height_inv );
		_uv.p2 = mt::vec2f( float( r.left + _width ) * atlas_width_inv, float( r.top + _height ) * atlas_height_inv );
		_uv.p3 = mt::vec2f( float( r.left ) * atlas_width_inv, float( r.top + _height ) * atlas_height_inv );

		return texture;
	}
    //////////////////////////////////////////////////////////////////////////
    static uint32_t getnp2( uint32_t _n )
    {
        uint32_t atlas_index = 0;
        while( (_n >>= 1) != 0 )
        {
            ++atlas_index;
        }

        return atlas_index;
    }
	//////////////////////////////////////////////////////////////////////////
	TTFAtlas * TTFAtlasService::getAtlas_( uint32_t _dimension, uint32_t _channel )
	{
        uint32_t atlas_index = getnp2( _dimension );

        atlas_index = atlas_index > m_minAtlasPow ? atlas_index - m_minAtlasPow : 0;

		if( atlas_index > m_maxAtlasPow )
		{
			return nullptr;
		}

        const uint32_t unpow_channel_mask[] = {0, 0, 1, 0, 1};

        uint32_t unpow_channel = unpow_channel_mask[_channel];

		TVectorAtlases & atlases = m_atlasess[unpow_channel][atlas_index];

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
        new_atlas.channel = _channel;

//#   ifdef MENGINE_RENDER_TEXTURE_RGBA
//        PixelFormat format_select[] = {PF_A8, PF_A8B8G8R8, PF_A8B8G8R8, PF_A8B8G8R8};
//#	else
//        PixelFormat format_select[] = {PF_A8, PF_A8R8G8B8, PF_A8R8G8B8, PF_A8R8G8B8};
//#	endif
        
        //PixelFormat format = format_select[unpow_channel];

        PixelFormat format = PF_A8R8G8B8;

		RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE( m_serviceProvider )
			->createDynamicTexture( m_maxAtlasWidth, _dimension, 4, 1, format );

		new_atlas.texture = texture;

		uint32_t max_glyph_count = m_maxAtlasWidth / _dimension;

		new_atlas.indices.reserve( max_glyph_count );

		for( uint32_t i = max_glyph_count; i != 0; --i )
		{
			new_atlas.indices.push_back( i - 1 );
		}

		atlases.push_back( new_atlas );
		
		TTFAtlas & atlas = atlases.back();

		return &atlas;
	}
}