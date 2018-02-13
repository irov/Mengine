#	include "FontBase.h"

#   include "Core/IniUtil.h"

#   include "Logger/Logger.h"

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
	FontBase::~FontBase()
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
	bool FontBase::compileFont()
	{
		bool successful = this->incrementReference();

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void FontBase::releaseFont()
	{
		this->decrementReference();
	}
	//////////////////////////////////////////////////////////////////////////
	bool FontBase::_incrementZero()
	{
		bool successful = this->compile();

		return successful;
	}
	//////////////////////////////////////////////////////////////////////////
	void FontBase::_decrementZero()
	{
		this->release();
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
    bool FontBase::validateText( const ConstString & _key, const String & _text ) const
    {
        bool result = true;

        U32String ttf_codes;

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

            bool control = false;

            if( code == 0 )
            {
                continue;
            }
            else if( code == 7 )
            {
                continue;
            }
            else if( code == 8 )
            {
                continue;
            }
            else if( code == 9 )
            {
                continue;
            }
            else if( code == 10 )
            {
                control = true;
            }
            else if( code == 11 )
            {
                continue;
            }
            else if( code == 12 )
            {
                continue;
            }
            else if( code == 13 )
            {
                control = true;
            }
            else if( code == 26 )
            {
                continue;
            }
            else if( code == 27 )
            {
                continue;
            }
            else if( code == 127 )
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

            if( control == true )
            {
                continue;
            }
            
            ttf_codes.push_back( code );            
        }

        if( this->_validateGlyphes( ttf_codes ) == false )
        {
            LOGGER_ERROR( "Text '%s' fontName '%s' invalid"
                , _key.c_str()
                , this->getName().c_str()
            );

            return false;
        }

        return true;
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

			bool control = false;

			if( code == 0 )
			{
				continue;
			}
			else if( code == 7 )
			{
				continue;
			}
			else if( code == 8 )
			{
				continue;
			}
			else if( code == 9 )
			{
				continue;
			}
			else if( code == 10 )
			{
				control = true;
			}
			else if( code == 11 )
			{
				continue;
			}
			else if( code == 12 )
			{
				continue;
			}
			else if( code == 13 )
			{
				control = true;
			}
			else if( code == 26 )
			{
				continue;
			}
			else if( code == 27 )
			{
				continue;
			}
			else if( code == 127 )
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

			if( control == false )
			{
				if( this->_prepareGlyph( code ) == false )
				{
					continue;
				}
			}

			result.push_back( (char32_t)code );
		}

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool FontBase::initializeBase_( const IniUtil::IniStore & _ini )
	{
		ColourValue colourFont;
		if( IniUtil::getIniValue( _ini, m_name.c_str(), "ColorFont", colourFont ) == true )
		{
			this->setColourFont( colourFont );
		}

		ColourValue colourOutline;
		if( IniUtil::getIniValue( _ini, m_name.c_str(), "ColorOutline", colourOutline ) == true )
		{
			this->setColourOutline( colourOutline );
		}

		float lineOffset;
		if( IniUtil::getIniValue( _ini, m_name.c_str(), "LineOffset", lineOffset ) == true )
		{
			this->setLineOffset( lineOffset );
		}

		float charOffset;
		if( IniUtil::getIniValue( _ini, m_name.c_str(), "CharOffset", charOffset ) == true )
		{
			this->setCharOffset( charOffset );
		}

		return true;
	}
}