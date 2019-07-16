#include "FontBase.h"

#include "Kernel/Logger.h"

#include "utf8.h"

namespace Mengine
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
    void FontBase::setType( const ConstString & _type )
    {
        m_type = _type;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & FontBase::getType() const
    {
        return m_type;
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
    bool FontBase::prefetch( const PrefetcherObserverInterfacePtr & _observer )
    {
        bool successful = this->_prefetch( _observer );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontBase::unfetch()
    {
        bool successful = this->_unfetch();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontBase::_prefetch( const PrefetcherObserverInterfacePtr & _observer )
    {
        _observer->onPrefetchComplete( true );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontBase::_unfetch()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontBase::setColorFont( const Color & _color )
    {
        m_colorFont = _color;

        m_params |= EFP_COLOR_FONT;
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
    const Color & FontBase::getFontColor() const
    {
        return m_colorFont;
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
        U32String ttf_codes;

        const Char * text_str = _text.c_str();
        size_t text_len = _text.size();

        for( const Char
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

        const Char * text_str = _text.c_str();
        size_t text_len = _text.size();

        for( const Char
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

            switch( code )
            {
            case 0:
            case 7:
            case 8:
            case 11:
            case 12:
            case 26:
            case 27:
            case 127:
                {
                    continue;
                }break;
            case 160:
                {
                    code = 32;
                }break;
            case 9:
                {
                    code = 32;
                }break;
            }

            result.push_back( (Char32)code );
        }

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontBase::prepareGlyph( const U32String & _text )
    {
        for( Char32 code : _text )
        {
            switch( code )
            {
            case 10:
            case 13:
                {
                    continue;
                }break;
            default:
                {
                }break;
            };

            if( this->_prepareGlyph( code ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontBase::initializeBase_( const ConfigInterfacePtr & _config )
    {
        Color colorFont;
        if( _config->hasValue( m_name.c_str(), "ColorFont", &colorFont ) == true )
        {
            this->setColorFont( colorFont );
        }

        float lineOffset;
        if( _config->hasValue( m_name.c_str(), "LineOffset", &lineOffset ) == true )
        {
            this->setLineOffset( lineOffset );
        }

        float charOffset;
        if( _config->hasValue( m_name.c_str(), "CharOffset", &charOffset ) == true )
        {
            this->setCharOffset( charOffset );
        }

        return true;
    }
}