#include "TextEntry.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TextEntry::TextEntry()
        : m_lineOffset( 0.f )
        , m_charOffset( 0.f )
        , m_maxLength( 0.f )
        , m_horizontAlign( ETFHA_LEFT )
        , m_verticalAlign( ETFVA_BOTTOM )
        , m_charScale( 1.f )
        , m_autoScale( false )
        , m_justify( false )
        , m_params( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TextEntry::~TextEntry()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextEntry::initialize( const ConstString & _key
        , const Char * _text
        , size_t _size
        , const Tags & _tags
        , const FontInterfacePtr & _font
        , const Color & _colorFont
        , float _lineOffset
        , float _charOffset
        , float _maxLength
        , ETextHorizontAlign _horizontAlign
        , ETextVerticalAlign _verticalAlign
        , float _charScale
        , bool _autoScale
        , bool _justify
        , uint32_t _params )
    {
        m_key = _key;

        if( _size != MENGINE_UNKNOWN_SIZE )
        {
            m_text.assign( _text, _size );
        }
        else
        {
            m_text.assign( _text );
        }

        m_tags = _tags;

        m_font = _font;

        m_colorFont = _colorFont;

        m_lineOffset = _lineOffset;
        m_charOffset = _charOffset;
        m_maxLength = _maxLength;

        m_horizontAlign = _horizontAlign;
        m_verticalAlign = _verticalAlign;

        m_charScale = _charScale;
        m_autoScale = _autoScale;
        m_justify = _justify;

        m_params = _params;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TextEntry::getKey() const
    {
        return m_key;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * TextEntry::getValue( size_t * const _size ) const
    {
        *_size = (size_t)m_text.size();

        const Char * text_str = m_text.c_str();

        return text_str;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & TextEntry::getTags() const
    {
        return m_tags;
    }
    //////////////////////////////////////////////////////////////////////////
    const FontInterfacePtr & TextEntry::getFont() const
    {
        return m_font;
    }
    //////////////////////////////////////////////////////////////////////////
    const Color & TextEntry::getColorFont() const
    {
        return m_colorFont;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextEntry::getLineOffset() const
    {
        return m_lineOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextEntry::getCharOffset() const
    {
        return m_charOffset;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextEntry::getMaxLength() const
    {
        return m_maxLength;
    }
    //////////////////////////////////////////////////////////////////////////
    ETextHorizontAlign TextEntry::getHorizontAlign() const
    {
        return m_horizontAlign;
    }
    //////////////////////////////////////////////////////////////////////////
    ETextVerticalAlign TextEntry::getVerticalAlign() const
    {
        return m_verticalAlign;
    }
    //////////////////////////////////////////////////////////////////////////
    float TextEntry::getCharScale() const
    {
        return m_charScale;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextEntry::getAutoScale() const
    {
        return m_autoScale;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TextEntry::getJustify() const
    {
        return m_justify;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t TextEntry::getFontParams() const
    {
        return m_params;
    }
    //////////////////////////////////////////////////////////////////////////
}