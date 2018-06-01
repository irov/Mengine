#include "TTFFont.h"

#include "TTFServiceInterface.h"

#include "Interface/RenderSystemInterface.h"
#include "Interface/FileSystemInterface.h"

#include "Core/Stream.h"
#include "Core/MemoryHelper.h"
#include "Core/IniUtil.h"

#include "Logger/Logger.h"

#include "utf8.h"

#include <algorithm>

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	TTFFont::TTFFont()
		: m_ftlibrary( nullptr )
		, m_face( nullptr )
		, m_ttfAscender( 0.f )
        , m_ttfDescender( 0.f )
        , m_ttfHeight( 0 )
        , m_ttfSpacing( 0.f )
        , m_ttfLayoutCount( 1 )
        , m_ttfFEBundle( nullptr )
        , m_ttfFEEffect( nullptr )
        , m_ttfFESample( 1 )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TTFFont::~TTFFont()
	{		
	}
	//////////////////////////////////////////////////////////////////////////
	void TTFFont::setFTLibrary( FT_Library _ftlibrary )
	{
		m_ftlibrary = _ftlibrary;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TTFFont::initialize( const ConstString & _category, const IniUtil::IniStore & _ini )
	{
		m_category = _category;

		if( this->initializeBase_( _ini ) == false )
		{
			return false;
		}

		if( IniUtil::getIniValue( _ini, m_name.c_str(), "Path", m_ttfPath ) == false )
		{
			LOGGER_ERROR("TextManager::loadFonts invalid font '%s' don't setup Glyph"
				, m_name.c_str()
				);

			return false;
		}

		if( IniUtil::getIniValue( _ini, m_name.c_str(), "Height", m_ttfHeight ) == false )
		{
			LOGGER_ERROR("TextManager::loadFonts invalid font '%s' don't setup Height"
				, m_name.c_str()
				);

			return false;
		}

        if( IniUtil::getIniValue( _ini, m_name.c_str(), "FEPath", m_ttfFEPath ) == true )
        {
            if( IniUtil::getIniValue( _ini, m_name.c_str(), "FEName", m_ttfFEName ) == false )
            {
                LOGGER_ERROR( "TextManager::loadFonts invalid font '%s' don't setup FEName"
                    , m_name.c_str()
                );

                return false;
            }
        }

        IniUtil::getIniValue( _ini, m_name.c_str(), "FESample", m_ttfFESample );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TTFFont::_compile()
	{
        if( m_ttfFEPath.empty() == false )
        {
            MemoryInterfacePtr ttfEffectMemory = Helper::createMemoryFile( m_category, m_ttfFEPath, false, __FILE__, __LINE__ );

            if( ttfEffectMemory == nullptr )
            {
                LOGGER_ERROR( "TTFFont::_compile font '%s' invalid get FE '%s'"
                    , m_name.c_str()
                    , m_ttfFEPath.c_str()
                );

                return false;
            }

            const void * ttfEffectMemory_buffer = ttfEffectMemory->getMemory();
            size_t ttfEffectMemory_size = ttfEffectMemory->getSize();

            m_ttfFEBundle = fe_bundle_load( (const uint8_t *)ttfEffectMemory_buffer, ttfEffectMemory_size );

            if( m_ttfFEBundle == nullptr )
            {
                return false;
            }

            m_ttfFEEffect = fe_bundle_get_effect_by_name( m_ttfFEBundle, m_ttfFEName.c_str() );

            if( m_ttfFEEffect == nullptr )
            {
                return false;
            }

            fe_node * effect_node_out = fe_effect_find_node_by_type( m_ttfFEEffect, fe_node_type_out );

            if( effect_node_out->properties_int[0] != 0 )
            {
                uint32_t layoutCount = 0;

                for( int i = 0; i < FE_MAX_PINS; ++i )
                {
                    const fe_node * effect_node_layout = effect_node_out->in[FE_MAX_PINS - i - 1].node;

                    if( effect_node_layout != nullptr )
                    {
                        m_ttfEffectNodes[layoutCount] = effect_node_layout;
                        ++layoutCount;
                    }
                }

                m_ttfLayoutCount = layoutCount;
            }
            else
            {
                m_ttfLayoutCount = 1;
            }
        }

		MemoryInterfacePtr memory = Helper::createMemoryFile( m_category, m_ttfPath, false, __FILE__, __LINE__ );

		FT_Byte * memory_byte = memory->getMemory();
		size_t memory_size = memory->getSize();

		if( memory_byte == nullptr )
		{
			return false;
		}

		FT_Error err_code = FT_New_Memory_Face( m_ftlibrary, memory_byte, memory_size, 0, &m_face );

		if( err_code != 0 )
		{
			LOGGER_ERROR("TTFFont::_compile invalid FT_New_Memory_Face font '%s' path '%s'"
				, m_name.c_str()
				, m_ttfPath.c_str()
				);

			return false;
		}

		if( FT_Select_Charmap( m_face, FT_ENCODING_UNICODE ) != FT_Err_Ok )
		{
			return false;
		}

        if( FT_Set_Pixel_Sizes( m_face, 0, m_ttfHeight * m_ttfFESample ) != FT_Err_Ok )
        {
            LOGGER_ERROR( "TTFFont::_compile font '%s' invalid set pixel height '%u'"
                , m_name.c_str()
                , m_ttfHeight
            );

            return false;
        }

		m_memory = memory;

        FT_Pos ascender = m_face->size->metrics.ascender >> 6;
        FT_Pos descender = m_face->size->metrics.descender >> 6;
        FT_Pos height = m_face->size->metrics.height >> 6;

        if( m_ttfFESample == 2 )
        {
            ascender >>= 1;
            descender >>= 1;
            height >>= 1;
        }

        m_ttfAscender = static_cast<float>(ascender);
        m_ttfDescender = -static_cast<float>(descender);

        float fHeight = static_cast<float>(height);
        m_ttfSpacing = fHeight - (m_ttfAscender + m_ttfDescender);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void TTFFont::_release()
	{
		FT_Done_Face( m_face );

		m_memory = nullptr;

        if( m_ttfFEBundle != nullptr )
        {
            fe_bundle_free( m_ttfFEBundle );
            m_ttfFEBundle = nullptr;
        }
	}
	//////////////////////////////////////////////////////////////////////////
    class TTFFont::PFindGlyph
    {
    public:
        PFindGlyph( GlyphCode _ch )
            : m_ch( _ch )
        {
        }

    public:
        bool operator () ( const TTFGlyph & _glyph ) const
        {
            return m_ch == _glyph.ch;
        }

    protected:
        GlyphCode m_ch;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        class TTFFontTextureGlyphProvider
            : public TextureGlyphProviderInterface
        {
        public:
            TTFFontTextureGlyphProvider( uint32_t _width, uint32_t _height, const void * _ttfbuffer, size_t _ttfpitch, uint32_t _ttfchannel )
                : m_width( _width )
                , m_height( _height )
                , m_ttfbuffer( _ttfbuffer )
                , m_ttfpitch( _ttfpitch )
                , m_ttfchannel( _ttfchannel )
            {
            }

        public:
            bool onTextureGlyphFill( uint8_t * _memory, size_t _pitch, uint32_t _channel ) override
            {
                const uint8_t * glyph_buffer = reinterpret_cast<const uint8_t *>(m_ttfbuffer);

                uint32_t border_width = m_width + 2;
                uint32_t border_height = m_height + 2;

                uint8_t * it_texture_memory = _memory;
                const uint8_t * it_glyph_buffer = glyph_buffer;

                it_texture_memory += _pitch; //y offset one pixel
                it_texture_memory += _channel; //x offset one pixel
                                
                if( m_ttfchannel == 1 && _channel == 4 )
                {
                    for( uint32_t h = 0; h != m_height; ++h )
                    {
                        for( uint32_t w = 0; w != m_width; ++w )
                        {
                            uint8_t * it_texture_memory_place = it_texture_memory + w * 4;

                            const uint8_t glyph_pixel = *(it_glyph_buffer + w);

                            *it_texture_memory_place++ = 255;
                            *it_texture_memory_place++ = 255;
                            *it_texture_memory_place++ = 255;
                            *it_texture_memory_place++ = glyph_pixel;
                        }

                        it_texture_memory += _pitch;
                        it_glyph_buffer += m_ttfpitch;
                    }
                }
                else if( m_ttfchannel == 4 && _channel == 4 )
                {
                    for( uint32_t h = 0; h != m_height; ++h )
                    {
                        stdex::memorycopy_pod( it_texture_memory, 0, it_glyph_buffer, m_width * m_ttfchannel );

                        it_texture_memory += _pitch;
                        it_glyph_buffer += m_ttfpitch;
                    }
                }
                else
                {
                    return false;
                }

                uint8_t * it_top_border_texture_memory = _memory;
                for( uint32_t w = 0; w != border_width; ++w )
                {
                    uint8_t * it_texture_memory_place = it_top_border_texture_memory;

                    *it_texture_memory_place++ = 0;
                    *it_texture_memory_place++ = 0;
                    *it_texture_memory_place++ = 0;
                    *it_texture_memory_place++ = 0;

                    it_top_border_texture_memory += _channel;
                }

                uint8_t * it_bottom_border_texture_memory = _memory + _pitch * border_height - _pitch;
                for( uint32_t w = 0; w != border_width; ++w )
                {
                    uint8_t * it_texture_memory_place = it_bottom_border_texture_memory;

                    *it_texture_memory_place++ = 0;
                    *it_texture_memory_place++ = 0;
                    *it_texture_memory_place++ = 0;
                    *it_texture_memory_place++ = 0;

                    it_bottom_border_texture_memory += _channel;
                }

                uint8_t * it_left_border_texture_memory = _memory + _pitch;
                for( uint32_t h = 0; h != m_height; ++h )
                {
                    uint8_t * it_texture_memory_place = it_left_border_texture_memory;

                    *it_texture_memory_place++ = 0;
                    *it_texture_memory_place++ = 0;
                    *it_texture_memory_place++ = 0;
                    *it_texture_memory_place++ = 0;

                    it_left_border_texture_memory += _pitch;
                }

                uint8_t * it_right_border_texture_memory = _memory + _pitch + _channel + m_width * _channel;
                for( uint32_t h = 0; h != m_height; ++h )
                {
                    uint8_t * it_texture_memory_place = it_right_border_texture_memory;

                    *it_texture_memory_place++ = 0;
                    *it_texture_memory_place++ = 0;
                    *it_texture_memory_place++ = 0;
                    *it_texture_memory_place++ = 0;

                    it_right_border_texture_memory += _pitch;
                }

                return true;
            }

        protected:
            uint32_t m_width;
            uint32_t m_height;

            const void * m_ttfbuffer;
            size_t m_ttfpitch;
            uint32_t m_ttfchannel;
        };
	}
	//////////////////////////////////////////////////////////////////////////
    bool TTFFont::_prepareGlyph( GlyphCode _code )
    {
        uint32_t code_hash = _code % MENGINE_TTF_FONT_GLYPH_HASH_SIZE;
        TVectorTTFGlyphs & glyphs = m_glyphsHash[code_hash];

        TVectorTTFGlyphs::iterator it_found = std::find_if( glyphs.begin(), glyphs.end(), PFindGlyph( _code ) );

        if( it_found != glyphs.end() )
        {
            const TTFGlyph & g = *it_found;
            (void)g;
            
            return true;
        }

        FT_UInt glyph_index = FT_Get_Char_Index( m_face, _code );

        if( FT_Load_Glyph( m_face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT | FT_LOAD_COLOR ) )
		{
			return false;
		}

		FT_GlyphSlot glyph = m_face->glyph;

		const FT_Glyph_Metrics & metrics = glyph->metrics;

        int32_t glyph_dx = (metrics.horiBearingX >> 6);
        int32_t glyph_dy = (metrics.horiBearingY >> 6);
        uint32_t glyph_w = (metrics.width >> 6);
        uint32_t glyph_h = (metrics.height >> 6);

		float glyph_advance = static_cast<float>(metrics.horiAdvance >> 6);

		FT_Bitmap glyph_bitmap = glyph->bitmap;

        uint32_t bitmap_channel = 0;

        switch( glyph_bitmap.pixel_mode )
        {
        case FT_PIXEL_MODE_MONO:
            {
                bitmap_channel = 1;
            }break;
        case FT_PIXEL_MODE_GRAY:
            {
                bitmap_channel = 1;
            }break;
        case FT_PIXEL_MODE_GRAY2:
            {
                bitmap_channel = 2;
            }break;
        case FT_PIXEL_MODE_GRAY4:
            {
                bitmap_channel = 4;
            }break;
        case FT_PIXEL_MODE_BGRA:
            {
                bitmap_channel = 4;
            }break;
        case FT_PIXEL_MODE_NONE:
        case FT_PIXEL_MODE_LCD:
        case FT_PIXEL_MODE_LCD_V:
            {
                return false;
            }break;
        };

        TTFGlyph ttf_glyph;
        ttf_glyph.ch = _code;
        ttf_glyph.advance = glyph_advance / m_ttfFESample;

        if( glyph_bitmap.width == 0 || glyph_bitmap.rows == 0 )
		{
            for( uint32_t index = 0; index != m_ttfLayoutCount; ++index )
            {
                TTFGlyphQuad & quad = ttf_glyph.quads[index];

                quad.offset.x = 0.f;
                quad.offset.y = 0.f;
                quad.size.x = 0.f;
                quad.size.y = 0.f;

                mt::uv4_identity( quad.uv );
                quad.texture = nullptr;
            }

			glyphs.emplace_back( ttf_glyph );

			return true;
		}        
        
        if( m_ttfFEEffect != nullptr )
        {
            int im_image_w = glyph_bitmap.width;
            int im_image_h = glyph_bitmap.rows;
            int im_image_pitch = glyph_bitmap.pitch;
            const void * im_image_data = glyph_bitmap.buffer;

            FE_IMAGE_FORMAT im_image_format = TF_UNDEFINED;
            switch( bitmap_channel )
            {
            case 1:
                im_image_format = FE_IMG_A8;
                break;
            case 4:
                im_image_format = FE_IMG_R8G8B8A8;
                break;
            }

            FT_Int im_x = glyph->bitmap_left;
            FT_Int im_y = glyph->bitmap_top;

            for( uint32_t layoutIndex = 0; layoutIndex != m_ttfLayoutCount; ++layoutIndex )
            {
                const fe_node * effect_node_layout = m_ttfEffectNodes[layoutIndex];

                fe_im res;
                fe_node_apply( m_ttfHeight * m_ttfFESample
                    , im_x
                    , im_y
                    , im_image_w
                    , im_image_h
                    , im_image_format
                    , im_image_pitch
                    , im_image_data
                    , effect_node_layout, &res );

                fe_image res_bgra;

#ifdef MENGINE_RENDER_TEXTURE_RGBA
                fe_image_create( &res_bgra, res.image.w, res.image.h, FE_IMG_R8G8B8A8 );
#else
                fe_image_create( &res_bgra, res.image.w, res.image.h, FE_IMG_B8G8R8A8 );
#endif

                fe_image_blit( &res.image, &res_bgra );

                fe_image_free( &res.image );

                switch( m_ttfFESample )
                {
                case 1:
                    {
                        TTFFontTextureGlyphProvider provider( res_bgra.w, res_bgra.h, res_bgra.data, res_bgra.pitch, res_bgra.bytespp );

                        mt::uv4f uv;
                        RenderTextureInterfacePtr texture = TTFATLAS_SERVICE()
                            ->makeTextureGlyph( res_bgra.w, res_bgra.h, res_bgra.bytespp, &provider, uv );

                        fe_image_free( &res_bgra );

                        if( texture == nullptr )
                        {
                            return false;
                        }

                        TTFGlyphQuad & quad = ttf_glyph.quads[layoutIndex];

                        quad.offset.x = (float)res.x;
                        quad.offset.y = (float)-res.y;
                        quad.size.x = (float)res_bgra.w;
                        quad.size.y = (float)res_bgra.h;
                        quad.uv = uv;
                        quad.texture = texture;
                    }break;
                case 2:
                    {
                        uint32_t downsample_w = res_bgra.w / 2;
                        uint32_t downsample_h = res_bgra.h / 2;

                        uint8_t * downsample_data = (uint8_t *)Helper::allocateMemory( downsample_w * downsample_h * res_bgra.bytespp, "TTFFont::_prepareGlyph" );

                        for( uint32_t h = 0; h != downsample_h; ++h )
                        {
                            uint32_t hh = h << 1;
                            uint32_t hh0 = (hh + 0) * res_bgra.pitch;
                            uint32_t hh1 = (hh + 1) * res_bgra.pitch;

                            for( uint32_t w = 0; w != downsample_w; ++w )
                            {
                                uint32_t ww = w << 1;
                                uint32_t ww0 = (ww + 0) << 2;
                                uint32_t ww1 = (ww + 1) << 2;

                                uint32_t r0 = res_bgra.data[ww0 + hh0 + 0];
                                uint32_t r1 = res_bgra.data[ww1 + hh0 + 0];
                                uint32_t r2 = res_bgra.data[ww1 + hh1 + 0];
                                uint32_t r3 = res_bgra.data[ww0 + hh1 + 0];

                                uint32_t r = (r0 + r1 + r2 + r3) >> 2;

                                uint32_t g0 = res_bgra.data[ww0 + hh0 + 1];
                                uint32_t g1 = res_bgra.data[ww1 + hh0 + 1];
                                uint32_t g2 = res_bgra.data[ww1 + hh1 + 1];
                                uint32_t g3 = res_bgra.data[ww0 + hh1 + 1];

                                uint32_t g = (g0 + g1 + g2 + g3) >> 2;

                                uint32_t b0 = res_bgra.data[ww0 + hh0 + 2];
                                uint32_t b1 = res_bgra.data[ww1 + hh0 + 2];
                                uint32_t b2 = res_bgra.data[ww1 + hh1 + 2];
                                uint32_t b3 = res_bgra.data[ww0 + hh1 + 2];

                                uint32_t b = (b0 + b1 + b2 + b3) >> 2;

                                uint32_t a0 = res_bgra.data[ww0 + hh0 + 3];
                                uint32_t a1 = res_bgra.data[ww1 + hh0 + 3];
                                uint32_t a2 = res_bgra.data[ww1 + hh1 + 3];
                                uint32_t a3 = res_bgra.data[ww0 + hh1 + 3];

                                uint32_t a = (a0 + a1 + a2 + a3) >> 2;

                                uint32_t downsample_index = (w + h * downsample_w) << 2;

                                downsample_data[downsample_index + 0] = (uint8_t)r;
                                downsample_data[downsample_index + 1] = (uint8_t)g;
                                downsample_data[downsample_index + 2] = (uint8_t)b;
                                downsample_data[downsample_index + 3] = (uint8_t)a;
                            }
                        }

                        TTFFontTextureGlyphProvider provider( downsample_w, downsample_h, downsample_data, downsample_w * res_bgra.bytespp, res_bgra.bytespp );

                        mt::uv4f uv;
                        RenderTextureInterfacePtr texture = TTFATLAS_SERVICE()
                            ->makeTextureGlyph( downsample_w, downsample_h, res_bgra.bytespp, &provider, uv );

                        Helper::freeMemory( downsample_data, "TTFFont::_prepareGlyph" );

                        fe_image_free( &res_bgra );

                        if( texture == nullptr )
                        {
                            return false;
                        }

                        TTFGlyphQuad & quad = ttf_glyph.quads[layoutIndex];

                        quad.offset.x = (float)res.x / m_ttfFESample;
                        quad.offset.y = (float)-res.y / m_ttfFESample;
                        quad.size.x = (float)downsample_w;
                        quad.size.y = (float)downsample_h;
                        quad.uv = uv;
                        quad.texture = texture;
                    }break;
                default:
                    return false;
                }
            }            
        }
        else
        {
            TTFFontTextureGlyphProvider provider( glyph_bitmap.width, glyph_bitmap.rows, glyph_bitmap.buffer, glyph_bitmap.pitch, bitmap_channel );

            mt::uv4f uv;
            RenderTextureInterfacePtr texture = TTFATLAS_SERVICE()
                ->makeTextureGlyph( glyph_bitmap.width, glyph_bitmap.rows, bitmap_channel, &provider, uv );

            if( texture == nullptr )
            {
                return false;
            }

            TTFGlyphQuad & quad = ttf_glyph.quads[0];
            
            quad.offset.x = (float)glyph_dx;
            quad.offset.y = (float)-glyph_dy;
            quad.size.x = (float)glyph_w;
            quad.size.y = (float)glyph_h;
            quad.uv = uv;
            quad.texture = texture;
        }

        glyphs.emplace_back( ttf_glyph );
		
		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::_validateGlyphes( const U32String & _codes ) const
    {
        MemoryInterfacePtr memory = Helper::createMemoryFile( m_category, m_ttfPath, false, __FILE__, __LINE__ );

        FT_Byte * memory_byte = memory->getMemory();
        size_t memory_size = memory->getSize();

        if( memory_byte == nullptr )
        {
            return false;
        }

        FT_Face face;
        FT_Error err_code = FT_New_Memory_Face( m_ftlibrary, memory_byte, memory_size, 0, &face );

        if( err_code != 0 )
        {
            return false;
        }

        if( FT_Select_Charmap( face, FT_ENCODING_UNICODE ) != FT_Err_Ok )
        {
            return false;
        }

        if( FT_Set_Pixel_Sizes( face, 0, m_ttfHeight * m_ttfFESample ) != FT_Err_Ok )
        {
            return false;
        }

        bool successful = true;

        for( U32String::const_iterator
            it = _codes.begin(),
            it_end = _codes.end();
            it != it_end;
            ++it )
        {
            FT_ULong ttf_code = (FT_ULong)*it;

            FT_UInt glyph_index = FT_Get_Char_Index( face, ttf_code );

            if( FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT | FT_LOAD_COLOR ) )
            {
                LOGGER_ERROR( "ttf font '%s' not found glyph code '%d'"
                    , this->getName().c_str()
                    , ttf_code
                );

                successful = false;

                continue;
            }

            FT_GlyphSlot glyph = face->glyph;

            FT_Bitmap bitmap = glyph->bitmap;

            unsigned char pixel_mode = bitmap.pixel_mode;

            switch( pixel_mode )
            {
            case FT_PIXEL_MODE_NONE:
            case FT_PIXEL_MODE_LCD:
            case FT_PIXEL_MODE_LCD_V:
                {
                    LOGGER_ERROR( "ttf font '%s' glyph '%d' unsupport mode '%d'"
                        , this->getName().c_str()
                        , ttf_code
                        , pixel_mode
                    );

                    successful = false;
                }break;
            };
        }

        FT_Done_Face( face );

        return successful;
    }
	//////////////////////////////////////////////////////////////////////////
	bool TTFFont::hasGlyph( GlyphCode _code ) const
	{
		uint32_t code_hash = _code % MENGINE_TTF_FONT_GLYPH_HASH_SIZE;
		const TVectorTTFGlyphs & glyphs = m_glyphsHash[code_hash];

		TVectorTTFGlyphs::const_iterator it_found = std::find_if( glyphs.begin(), glyphs.end(), PFindGlyph( _code ) );

		if( it_found == glyphs.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TTFFont::getGlyph( uint32_t _layout, GlyphCode _code, GlyphCode _next, Glyph * _glyph ) const
	{
		uint32_t code_hash = _code % MENGINE_TTF_FONT_GLYPH_HASH_SIZE;
		const TVectorTTFGlyphs & glyphs = m_glyphsHash[code_hash];

		TVectorTTFGlyphs::const_iterator it_found = std::find_if( glyphs.begin(), glyphs.end(), PFindGlyph( _code ) );

		if( it_found == glyphs.end() )
		{
			return false;
		}

		const TTFGlyph & glyph = *it_found;

		_glyph->advance = glyph.advance;

        const TTFGlyphQuad & quad = glyph.quads[_layout];

        _glyph->offset = quad.offset;
        _glyph->size = quad.size;
        _glyph->uv = quad.uv;
        _glyph->texture = quad.texture;

		if( _next == 0 )
		{
			return true;
		}

		FT_UInt rindex = FT_Get_Char_Index( m_face, _next );

		if( rindex == 0 )
		{
			return true;
		}

		FT_Vector ttf_kerning;
		FT_Get_Kerning( m_face, _code, _next, FT_KERNING_DEFAULT, &ttf_kerning );

		if( ttf_kerning.x == 0 )
		{
			return true;
		}

		float kerning = (float)(ttf_kerning.x >> 6);

		_glyph->advance += kerning;

		return true;
	}
    //////////////////////////////////////////////////////////////////////////
    uint32_t TTFFont::getLayoutCount() const
    {
        return m_ttfLayoutCount;
    }
    //////////////////////////////////////////////////////////////////////////
    float TTFFont::getFontAscent() const
    {
        return m_ttfAscender;
    }
    //////////////////////////////////////////////////////////////////////////
    float TTFFont::getFontDescent() const
    {
        return m_ttfDescender;
    }
    //////////////////////////////////////////////////////////////////////////
    float TTFFont::getFontHeight() const
    {
        return m_ttfAscender + m_ttfDescender;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TTFFont::getFontPremultiply() const
    {
        if( m_ttfFEEffect != nullptr )
        {
            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    float TTFFont::getFontSpacing() const
    {
        return m_ttfSpacing;
    }
}
