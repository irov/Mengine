#	include "TTFFont.h"

#	include "TTFServiceInterface.h"

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/FileSystemInterface.h"

#   include "Core/Stream.h"
#	include "Core/MemoryHelper.h"
#   include "Core/IniUtil.h"

#	include "Logger/Logger.h"

#	include "utf8.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TTFFont::TTFFont()
		: m_ftlibrary( nullptr )
		, m_face( nullptr )
		, m_ttfAscender( 0.f )
        , m_ttfDescender( 0.f )
        , m_ttfHeight( 0 )
        , m_ttfSpacing( 0.f )
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
	namespace
	{
        //////////////////////////////////////////////////////////////////////////
		class PFindGlyph
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

        uint32_t bitmap_pitch = bitmap.pitch;
		uint32_t bitmap_width = bitmap.width;
        uint32_t bitmap_height = bitmap.rows;
		const void * bitmap_buffer = bitmap.buffer;

		if( bitmap_width == 0 || bitmap_height == 0 )
		{
			TTFGlyph g;
			g.ch = _code;
			g.advance = advance;

			g.dx = 0.f;
			g.dy = 0.f;
			g.w = 0.f;
			g.h = 0.f;

			g.texture = nullptr;

			glyphs.push_back( g );

			return true;
		}

        
        fe_im res;
        if( m_ttfFEEffect != nullptr )
        {
            fe_node * effect_node = fe_effect_find_node_by_type( m_ttfFEEffect, fe_node_type_out );
        
            fe_im im;
            im.image.w = bitmap_width;
            im.image.h = bitmap_height;
            im.image.pitch = bitmap_pitch;
            im.image.bytespp = bitmap_channel;
            im.image.data = static_cast<uint8_t *>(const_cast<void*>(bitmap_buffer));
            //im.image.data = (uint8_t *)bitmap_buffer;

            switch( bitmap_channel )
            {
            case 1:
                im.image.format = FE_IMG_A8;
                break;
            case 4:
                im.image.format = FE_IMG_R8G8B8A8;
                break;
            }
            
            im.x = glyph->bitmap_left;
            im.y = m_ttfHeight - glyph->bitmap_top;

            
            fe_node_apply( m_ttfHeight, &im, effect_node, &res );

            bitmap_width = res.image.w;
            bitmap_height = res.image.h;
            bitmap_buffer = res.image.data;
            bitmap_pitch = res.image.pitch;
            bitmap_channel = res.image.bytespp;

            dx = res.x;
            dy = res.y - m_ttfHeight;
        }
        
        TTFFontTextureGlyphProvider provider( bitmap_width, bitmap_height, bitmap_buffer, bitmap_pitch, bitmap_channel );
        
		mt::uv4f uv;
		RenderTextureInterfacePtr texture = TTFATLAS_SERVICE()
			->makeTextureGlyph( bitmap_width, bitmap_height, bitmap_channel, &provider, uv );

		if( texture == nullptr )
		{
			return false;
		}

        if( m_ttfFEEffect != nullptr )
        {
            fe_image_free( &res.image );
        }

		TTFGlyph g;
		g.ch = _code;
		g.advance = advance;

        g.dx = (float)dx;
        g.dy = (float)dy;
        g.w = (float)w;
        g.h = (float)h;

		g.texture = texture;
		g.uv = uv;

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
	bool TTFFont::getGlyph( GlyphCode _code, GlyphCode _next, Glyph * _glyph ) const
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
		_glyph->offset = mt::vec2f( glyph.dx, glyph.dy );
		_glyph->size = mt::vec2f( glyph.w, glyph.h );

		_glyph->texture = glyph.texture;
		_glyph->uv = glyph.uv;

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
