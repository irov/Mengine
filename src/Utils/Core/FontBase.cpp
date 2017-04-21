#	include "FontBase.h"

#	include "utf8.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	FontBase::FontBase()
		: m_params( EFP_NONE )
		, m_lineOffset( 0.f )
		, m_charOffset( 0.f )
	{
	}
	//////////////////////////////////////////////////////////////////////////	
	void FontBase::setName( const ConstString & _name )
	{
		m_name = _name;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & FontBase::getName() const
	{
		return m_name;
	}
	//////////////////////////////////////////////////////////////////////////
	void FontBase::setCategory( const ConstString & _category )
	{
		m_category = _category;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & FontBase::getCategory() const
	{
		return m_category;
	}
	//////////////////////////////////////////////////////////////////////////
	void FontBase::setColourFont( const ColourValue & _colour )
	{
		m_colourFont = _colour;

		m_params |= EFP_COLOR_FONT;
	}
	//////////////////////////////////////////////////////////////////////////
	void FontBase::setColourOutline( const ColourValue & _colour )
	{
		m_colourOutline = _colour;

		m_params |= EFP_COLOR_OUTLINE;
	}
	//////////////////////////////////////////////////////////////////////////
	void FontBase::setLineOffset( float _lineOffset )
	{
		m_lineOffset = _lineOffset;

		m_params |= EFP_LINE_OFFSET;
	}
	//////////////////////////////////////////////////////////////////////////
	void FontBase::setCharOffset( float _charOffset )
	{
		m_charOffset = _charOffset;

		m_params |= EFP_CHAR_OFFSET;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t FontBase::getFontParams() const
	{
		return m_params;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & FontBase::getColorFont() const
	{
		return m_colourFont;
	}
	//////////////////////////////////////////////////////////////////////////
	const ColourValue & FontBase::getColorOutline() const
	{
		return m_colourOutline;
	}
	//////////////////////////////////////////////////////////////////////////
	float FontBase::getLineOffset() const
	{
		return m_lineOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	float FontBase::getCharOffset() const
	{
		return m_charOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	U32String FontBase::prepareText( const String & _text )
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

			if( this->_prepareGlyph( code ) == false )
			{
				return 0;
			}

			result.push_back( code );
		}

		return result;
	}
}