#	include "TTFFont.h"

#	include "TTFAtlasServiceInterface.h"

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

		FT_Byte * memory_byte = _memory->getMemory();
		size_t memory_size = _memory->getSize();

		if( memory_byte == nullptr )
		{
			return false;
		}

		FT_Error err_code = FT_New_Memory_Face( m_library, memory_byte, memory_size, 0, &m_face );

		if( err_code != 0 )
		{
			return false;
		}

		m_ascender = static_cast<float>(m_face->size->metrics.ascender >> 6);

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool TTFFont::prepareText( const String & _text )
	{
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
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	const TTFGlyph * TTFFont::getGlyph_( WChar _ch )
	{
		uint32_t ch_hash = _ch % MENGINE_TTF_FONT_GLYPH_HASH_SIZE;

		TMapTTFGlyphs & glyphs = m_glyphsHash[ch_hash];

		TMapTTFGlyphs::iterator it_found = glyphs.find( _ch );

		if( it_found == glyphs.end() )
		{
			FT_Set_Char_Size( m_face, 0, 16 * 64, 300, 300 );

			if( FT_Load_Char( m_face, _ch, FT_LOAD_RENDER ) )
				throw std::runtime_error( "FT_Load_Glyph failed" );

			FT_GlyphSlot glyph = m_face->glyph;

			const FT_Glyph_Metrics & metrics = glyph->metrics;
			Rect outRect;
			outRect.left = metrics.horiBearingX >> 6;
			outRect.top = -(metrics.horiBearingY >> 6);
			outRect.right = outRect.left + (metrics.width >> 6);
			outRect.bottom = outRect.top + (metrics.height >> 6);

			m_advance = static_cast<float>(metrics.horiAdvance >> 6);

			FT_Bitmap bitmap = glyph->bitmap;
			uint32_t bitmap_width = bitmap.width;
			uint32_t bitmap_height = bitmap.rows;

			const void * buffer = glyph->bitmap.buffer;

			mt::uv4f uv;
			RenderTextureInterfacePtr texture = TTFATLAS_SERVICE( m_serviceProvider )
				->makeTextureGlyph( bitmap_width, bitmap_height, buffer, bitmap_width, uv );

			TTFGlyph g;
			g.texture = texture;
			g.uv = uv;

			it_found = glyphs.insert( std::make_pair( _ch, g ) ).first;
		}

		const TTFGlyph & glyph = it_found->second;

		return &glyph;
	}
}