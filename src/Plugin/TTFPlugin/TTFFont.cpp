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
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TTFFont::_compile()
	{
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

        if( FT_Set_Pixel_Sizes( m_face, 0, m_ttfHeight ) != FT_Err_Ok )
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

        m_ttfAscender = static_cast<float>(ascender);
        m_ttfDescender = -static_cast<float>(descender);

        float fHeight = static_cast<float>(height);
        m_ttfSpacing = fHeight - (m_ttfAscender + m_ttfDescender);

        if( m_ttfFEPath.empty() == false )
        {
            MemoryInterfacePtr ttfEffectMemory = Helper::createMemoryFile( m_category, m_ttfFEPath, false, __FILE__, __LINE__ );

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

            fe_node * effect_node = fe_effect_find_node_by_type( m_ttfFEEffect, fe_node_type_out );

            if( effect_node->properties_int[0] != 0 )
            {
                uint32_t layoutCount = 0;

                for( int i = 0; i < FE_MAX_PINS; ++i )
                {
                    const fe_node *left = effect_node->in[FE_MAX_PINS - i - 1].node;

                    if( left != nullptr )
                    {
                        ++layoutCount;
                    }
                }

                m_ttfLayoutCount = layoutCount;
            }
        }

        if( m_ttfLayoutCount > 4 )
        {
            LOGGER_ERROR( "TTFFont::_compile font '%s' layout more 4 - '%u'"
                , m_name.c_str()
                , m_ttfLayoutCount
            );

            return false;
        }

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
            TTFFontTextureGlyphProvider( uint32_t _width, uint32_t _height, const void * _ttfbuffer, size_t _ttfpitch, uint32_t _ttfchannel, bool _rgba )
                : m_width( _width )
                , m_height( _height )
                , m_ttfbuffer( _ttfbuffer )
                , m_ttfpitch( _ttfpitch )
                , m_ttfchannel( _ttfchannel )
                , m_rgba( _rgba )
            {
            }

        public:
            bool onTextureGlyphFill( uint8_t * _memory, size_t _pitch, uint32_t _channel ) override
            {
                const uint8_t * glyph_buffer = reinterpret_cast<const uint8_t *>(m_ttfbuffer);

                uint32_t border_width = m_width + 2;
                uint32_t border_height = m_height + 2;

                uint8_t * it_texture_memory = _memory + _channel;
                const uint8_t * it_glyph_buffer = glyph_buffer;

                it_texture_memory += _pitch;

                                
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
                    if( m_rgba == false )
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
                        for( uint32_t h = 0; h != m_height; ++h )
                        {
                            for( uint32_t w = 0; w != m_width; ++w )
                            {
                                uint8_t * it_texture_memory_place = it_texture_memory + w * 4;

                                const uint8_t * glyph_pixel = it_glyph_buffer + w * 4;

                                *it_texture_memory_place++ = glyph_pixel[2];
                                *it_texture_memory_place++ = glyph_pixel[1];
                                *it_texture_memory_place++ = glyph_pixel[0];
                                *it_texture_memory_place++ = glyph_pixel[3];

                                glyph_pixel += 4;
                            }

                            it_texture_memory += _pitch;
                            it_glyph_buffer += m_ttfpitch;
                        }
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

            bool m_rgba;
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

        int32_t dx = (metrics.horiBearingX >> 6);
        int32_t dy = -(metrics.horiBearingY >> 6);
        uint32_t w = (metrics.width >> 6);
        uint32_t h = (metrics.height >> 6);

		float advance = static_cast<float>(metrics.horiAdvance >> 6);

		FT_Bitmap bitmap = glyph->bitmap;

        uint32_t bitmap_channel = 0;

        switch( bitmap.pixel_mode )
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

        TTFGlyph g;
        g.ch = _code;
        g.advance = advance;

        if( bitmap.width == 0 || bitmap.rows == 0 )
		{
            for( uint32_t index = 0; index != m_ttfLayoutCount; ++index )
            {
                g.layout[index].dx = 0.f;
                g.layout[index].dy = 0.f;
                g.layout[index].w = 0.f;
                g.layout[index].h = 0.f;

                g.layout[index].texture = nullptr;
            }

			glyphs.push_back( g );

			return true;
		}
        
        fe_im res;
        if( m_ttfFEEffect != nullptr )
        {
            const fe_node * effect_node = fe_effect_find_node_by_type( m_ttfFEEffect, fe_node_type_out );

            const fe_node * effects[4];

            if( effect_node->properties_int[0] != 0 )
            {
                uint32_t layoutCount = 0;

                for( int i = 0; i < FE_MAX_PINS; ++i )
                {
                    const fe_node * left = effect_node->in[FE_MAX_PINS - i - 1].node;

                    if( left == nullptr )
                    {
                        continue;
                    }
                     
                    effects[layoutCount] = left;
                    ++layoutCount;                    
                }
            }
            else
            {
                effects[0] = effect_node;
            }

            for( uint32_t layoutIndex = 0; layoutIndex != m_ttfLayoutCount; ++layoutIndex )
            {
                int im_image_w = bitmap.width;
                int im_image_h = bitmap.rows;
                int im_image_pitch = bitmap.pitch;
                const void * im_image_data = bitmap.buffer;
                //im.image.data = (uint8_t *)bitmap_buffer;

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

                fe_node_apply( m_ttfHeight
                    , im_x
                    , im_y
                    , im_image_w
                    , im_image_h
                    , im_image_format
                    , im_image_pitch
                    , im_image_data
                    , effect_node, &res );

                TTFFontTextureGlyphProvider provider( res.image.w, res.image.h, res.image.data, res.image.pitch, res.image.bytespp, true );

                mt::uv4f uv;
                RenderTextureInterfacePtr texture = TTFATLAS_SERVICE()
                    ->makeTextureGlyph( res.image.w, res.image.h, res.image.bytespp, &provider, uv );

                if( texture == nullptr )
                {
                    return false;
                }

                fe_image_free( &res.image );
                
                g.layout[layoutIndex].dx = (float)res.x;
                g.layout[layoutIndex].dy = (float)res.y;
                g.layout[layoutIndex].w = (float)w;
                g.layout[layoutIndex].h = (float)h;
                g.layout[layoutIndex].uv = uv;
                g.layout[layoutIndex].texture = texture;
            }            
        }
        else
        {
            TTFFontTextureGlyphProvider provider( bitmap.width, bitmap.rows, bitmap.buffer, bitmap.pitch, bitmap_channel, false );

            mt::uv4f uv;
            RenderTextureInterfacePtr texture = TTFATLAS_SERVICE()
                ->makeTextureGlyph( bitmap.width, bitmap.rows, bitmap_channel, &provider, uv );

            if( texture == nullptr )
            {
                return false;
            }
            
            g.layout[0].dx = (float)dx;
            g.layout[0].dy = (float)dy;
            g.layout[0].w = (float)w;
            g.layout[0].h = (float)h;
            g.layout[0].uv = uv;
            g.layout[0].texture = texture;
        }

        glyphs.push_back( g );
		
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

        if( FT_Set_Pixel_Sizes( face, 0, m_ttfHeight ) != FT_Err_Ok )
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
		_glyph->offset = mt::vec2f( glyph.layout[_layout].dx, glyph.layout[_layout].dy );
		_glyph->size = mt::vec2f( glyph.layout[_layout].w, glyph.layout[_layout].h );
        _glyph->uv = glyph.layout[_layout].uv;
		_glyph->texture = glyph.layout[_layout].texture;		

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
