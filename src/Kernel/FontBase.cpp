#include "FontBase.h"

#include "Kernel/Logger.h"

#include "utf8.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    FontBase::FontBase()
        : m_params( EFP_NONE )
        , m_height( 0 )
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
        bool successful = this->compile();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontBase::releaseFont()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontBase::isCompileFont() const
    {
        return this->isCompile();
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
        _observer->onPrefetchPreparation();
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
    void FontBase::setHeight( uint32_t _height )
    {
        m_height = _height;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t FontBase::getHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontBase::setFontColor( const Color & _color )
    {
        m_colorFont = _color;

        m_params |= EFP_COLOR_FONT;
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & FontBase::getFontColor() const
    {
        return m_colorFont;
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
    bool FontBase::validateText( const ConstString & _key, const Char * _text, size_t _size ) const
    {
        U32String ttf_codes;

        const Char * text_str = _text;
        size_t text_len = _size;

        for( const Char
            * text_it = text_str,
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
    bool FontBase::prepareText( const Char * _text, size_t _size, U32String * const _out ) const
    {
        const Char * text_str = _text;
        size_t text_len = _size;

        for( const Char
            * text_it = text_str,
            *text_end = text_str + text_len + 1;
            text_it != text_end;
            )
        {
            uint32_t code = 0;
            utf8::internal::utf_error err = utf8::internal::validate_next( text_it, text_end, code );

            if( err != utf8::internal::UTF8_OK )
            {
                LOGGER_ERROR( "text '%.*s' invalid utf8 [%u]"
                    , _size
                    , _text
                    , err 
                );

                return false;
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
            default:
                break;
            }

            _out->push_back( (Char32)code );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontBase::prepareGlyph( const U32String & _text, const DocumentPtr & _doc )
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

            if( this->_prepareGlyph( code, _doc ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}