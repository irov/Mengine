#	include "TTFFont.h"

#	include "TTFServiceInterface.h"

#	include "utf8.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TTFFont::TTFFont()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TTFFont::~TTFFont()
	{
		FT_Done_Face( m_face );
	}
	//////////////////////////////////////////////////////////////////////////
	bool TTFFont::initialize( FT_Library _library, const MemoryInterfacePtr & _memory )
	{
		m_library = _library;
        m_memory = _memory;

		FT_Byte * memory_byte = m_memory->getMemory();
		size_t memory_size = m_memory->getSize();

		if( memory_byte == nullptr )
		{
			return false;
		}

		FT_Error err_code = FT_New_Memory_Face( m_library, memory_byte, memory_size, 0, &m_face );

		if( err_code != 0 )
		{
			return false;
		}

        if( FT_Select_Charmap( m_face, FT_ENCODING_UNICODE ) != FT_Err_Ok )
        {
            return false;
        }

        FT_F26Dot6 fontSizePoints = 64 * 64;
        FT_UInt dpi = 72;
        if( FT_Set_Char_Size( m_face, fontSizePoints, fontSizePoints, dpi, dpi ) != FT_Err_Ok )
        {
            return false;
        }

        m_ascender = static_cast<float>(m_face->size->metrics.ascender >> 6);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	U32String TTFFont::prepareText( const String & _text )
	{
		U32String result;

		const char * text_str = _text.c_str();
		size_t text_len = _text.size();

		for( const char
			*text_it = text_str,
			*text_end = text_str + text_len + 1;
		text_it != text_end;
		)
		{
			uint32_t code = 0;
			utf8::internal::utf_error err = utf8::internal::validate_next( text_it, text_end, code );

			if( err != utf8::internal::UTF8_OK )
			{
				continue;
			}

			if( code == 0 )
			{
				continue;
			}
			else if( code == 10 )
			{
				continue;
			}
			else if( code == 13 )
			{
				continue;
			}
			else if( code == 160 )
			{
				code = 32;
			}
			else if( code == 9 )
			{
				code = 32;
			}

			if( this->prepareGlyph_( code ) == false )
			{
				return false;
			}

			result.push_back( code );
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
		class PFindGlyph
		{
		public:
			PFindGlyph( uint32_t _ch )
				: m_ch( _ch )
			{
			}

		public:
			bool operator () ( const TTFGlyph & _glyph ) const
			{
				return m_ch == _glyph.ch;
			}

		protected:
			uint32_t m_ch;
		};
	}
	//////////////////////////////////////////////////////////////////////////
    bool TTFFont::prepareGlyph_( uint32_t _ch )
    {
        uint32_t ch_hash = _ch % MENGINE_TTF_FONT_GLYPH_HASH_SIZE;

        TMapTTFGlyphs & glyphs = m_glyphsHash[ch_hash];

        TMapTTFGlyphs::iterator it_found = std::find_if( glyphs.begin(), glyphs.end(), PFindGlyph( _ch ) );

        if( it_found != glyphs.end() )
        {
            return true;
        }

        FT_UInt glyph_index = FT_Get_Char_Index( m_face, _ch );

        if( FT_Load_Glyph( m_face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_AUTOHINT ) )
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

		const void * buffer = glyph->bitmap.buffer;

		mt::uv4f uv;
		RenderTextureInterfacePtr texture = TTFATLAS_SERVICE( m_serviceProvider )
			->makeTextureGlyph( bitmap_width, bitmap_height, bitmap_channel, buffer, bitmap_pitch, uv );

		if( texture == nullptr )
		{
			return false;
		}

		TTFGlyph g;
		g.ch = _ch;
		g.advance = advance;

        g.dx = (float)dx;
        g.dy = (float)dy;
        g.ax = (float)dx + w;
        g.ay = (float)dy + h;

		g.texture = texture;
		g.uv = uv;

		glyphs.push_back( g );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const TTFGlyph * TTFFont::getGlyph( uint32_t _ch ) const
	{
		uint32_t ch_hash = _ch % MENGINE_TTF_FONT_GLYPH_HASH_SIZE;

		const TMapTTFGlyphs & glyphs = m_glyphsHash[ch_hash];

		TMapTTFGlyphs::const_iterator it_found = std::find_if( glyphs.begin(), glyphs.end(), PFindGlyph( _ch ) );

		if( it_found == glyphs.end() )
		{
			return nullptr;
		}

		const TTFGlyph & glyph = *it_found;

		return &glyph;
	}
}