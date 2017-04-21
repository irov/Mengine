#	include "TTFFont.h"

#	include "TTFServiceInterface.h"

#	include "Interface/FileSystemInterface.h"
#	include "Interface/MemoryInterface.h"

#	include "Core/MemoryHelper.h"

#	include "Logger/Logger.h"

#	include "utf8.h"

#	include <algorithm>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	TTFFont::TTFFont()
		: m_library(nullptr)
		, m_face(nullptr)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	TTFFont::~TTFFont()
	{
		FT_Done_Face( m_face );
	}
	//////////////////////////////////////////////////////////////////////////
	void TTFFont::setFTLibrary( FT_Library _library )
	{
		m_library = _library;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TTFFont::initialize( const ConstString & _category, const IniUtil::IniStore & _ini )
	{
		if( this->initializeBase_( _ini ) == false )
		{
			return false;
		}

		FilePath ttfPath;
		if( IniUtil::getIniValue( _ini, m_name.c_str(), "Path", ttfPath, m_serviceProvider ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("TextManager::loadFonts invalid font %s don't setup Glyph"
				, m_name.c_str()
				);

			return false;
		}

		InputStreamInterfacePtr stream = FILE_SERVICE( m_serviceProvider )
			->openInputFile( _category, ttfPath, false );

		if( stream == nullptr )
		{
			return false;
		}

		MemoryInterfacePtr memory = Helper::createMemoryStream( m_serviceProvider, stream, __FILE__, __LINE__ );
		
		FT_Byte * memory_byte = memory->getMemory();
		size_t memory_size = memory->getSize();

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

		uint32_t fontHeight = 64;
		IniUtil::getIniValue( _ini, m_name.c_str(), "Height", fontHeight, m_serviceProvider );
		
		uint32_t fontDPI = 72;
		IniUtil::getIniValue( _ini, m_name.c_str(), "DPI", fontDPI, m_serviceProvider );
		
        FT_F26Dot6 fontSizePoints = (FT_F26Dot6)fontHeight * 64;
        FT_UInt dpi = (FT_UInt)fontDPI;
        if( FT_Set_Char_Size( m_face, fontSizePoints, fontSizePoints, dpi, dpi ) != FT_Err_Ok )
        {
            return false;
        }

		m_height = (float)fontHeight;
        m_ascender = static_cast<float>(m_face->size->metrics.ascender >> 6);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	namespace
	{
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
	}
	//////////////////////////////////////////////////////////////////////////
    bool TTFFont::_prepareGlyph( GlyphCode _ch )
    {
        uint32_t ch_hash = _ch % MENGINE_TTF_FONT_GLYPH_HASH_SIZE;

        TVectorTTFGlyphs & glyphs = m_glyphsHash[ch_hash];

        TVectorTTFGlyphs::iterator it_found = std::find_if( glyphs.begin(), glyphs.end(), PFindGlyph( _ch ) );

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
	bool TTFFont::hasGlyph( GlyphCode _char ) const
	{
		uint32_t ch_hash = _char % MENGINE_TTF_FONT_GLYPH_HASH_SIZE;

		const TVectorTTFGlyphs & glyphs = m_glyphsHash[ch_hash];

		TVectorTTFGlyphs::const_iterator it_found = std::find_if( glyphs.begin(), glyphs.end(), PFindGlyph( _char ) );

		if( it_found == glyphs.end() )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TTFFont::getGlyph( GlyphCode _char, GlyphCode _next, Glyph * _glyph ) const
	{
		uint32_t ch_hash = _char % MENGINE_TTF_FONT_GLYPH_HASH_SIZE;

		const TVectorTTFGlyphs & glyphs = m_glyphsHash[ch_hash];

		TVectorTTFGlyphs::const_iterator it_found = std::find_if( glyphs.begin(), glyphs.end(), PFindGlyph( _char ) );

		if( it_found == glyphs.end() )
		{
			return false;
		}

		const TTFGlyph & glyph = *it_found;

		_glyph->advance = glyph.advance;
		_glyph->offset = mt::vec2f( glyph.dx, glyph.dy );
		_glyph->size = mt::vec2f( glyph.ax, glyph.ay );

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
		FT_Get_Kerning( m_face, _char, _next, FT_KERNING_DEFAULT, &ttf_kerning );

		if( ttf_kerning.x == 0 )
		{
			return true;
		}

		float kerning = (float)(ttf_kerning.x >> 6);

		_glyph->advance += kerning;

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	float TTFFont::getFontHeight() const
	{
		return m_height;
	}
}