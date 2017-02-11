#	include "TTFFont.h"

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

			FT_Set_Char_Size( m_face, 0, 16 * 64, 300, 300 );

			if( FT_Load_Glyph( m_face, FT_Get_Char_Index( m_face, code ), FT_LOAD_RENDER ) )
				throw std::runtime_error( "FT_Load_Glyph failed" );

			FT_GlyphSlot g = m_face->glyph;
			
			float dx = (float)g->bitmap_left;
			float dy = (float)g->bitmap_top;
		}
	}
}