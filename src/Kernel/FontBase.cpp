#include "FontBase.h"

#include "Kernel/Logger.h"
#include "Kernel/Utf8Helper.h"

//////////////////////////////////////////////////////////////////////////
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
    void FontBase::setGlyph( const FontGlyphInterfacePtr & _glyph )
    {
        m_glyph = _glyph;
    }
    //////////////////////////////////////////////////////////////////////////
    const FontGlyphInterfacePtr & FontBase::getGlyph() const
    {
        return m_glyph;
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
    void FontBase::unfetch()
    {
        this->_unfetch();
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontBase::_prefetch( const PrefetcherObserverInterfacePtr & _observer )
    {
        MENGINE_UNUSED( _observer );

        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void FontBase::_unfetch()
    {
        //Empty
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
            uint32_t code;
            if( Helper::Utf8NextCode( &text_it, text_end, &code ) == false )
            {
                LOGGER_ERROR( "font '%s' text '%s' have invalid utf8 symbol '%s'"
                    , this->getName().c_str()
                    , _text
                    , text_it 
                );

                return false;
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
            LOGGER_ERROR( "font '%s' text '%s' invalid glyphes"
                , this->getName().c_str()
                , _key.c_str()
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
            uint32_t code;
            if( Helper::Utf8NextCode( &text_it, text_end, &code ) == false )
            {
                LOGGER_ERROR( "font '%s' text '%.*s' invalid utf8"
                    , this->getName().c_str()
                    , (int32_t)_size
                    , _text
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
    bool FontBase::prepareGlyph( const U32String & _text, const DocumentInterfacePtr & _doc )
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