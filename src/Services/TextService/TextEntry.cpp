#include "TextEntry.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TextEntry::TextEntry()
        : m_lineOffset( 0.f )
        , m_charOffset( 0.f )
        , m_maxLength( -1.f )
        , m_maxHeight( -1.f )
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
    bool TextEntry::initialize( const ConstString & _key, const TextEntryDesc & _desc )
    {
        m_key = _key;

        if( _desc.size != MENGINE_UNKNOWN_SIZE )
        {
            m_text.assign( _desc.text, _desc.size );
        }
        else
        {
            m_text.assign( _desc.text );
        }

        m_tags = _desc.tags;

        m_font = _desc.font;

        m_colorFont = _desc.colorFont;

        m_lineOffset = _desc.lineOffset;
        m_charOffset = _desc.charOffset;
        m_maxLength = _desc.maxLength;
        m_maxHeight = _desc.maxHeight;

        m_horizontAlign = _desc.horizontAlign;
        m_verticalAlign = _desc.verticalAlign;

        m_charScale = _desc.charScale;
        m_autoScale = _desc.autoScale;
        m_justify = _desc.justify;

        m_params = _desc.params;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & TextEntry::getKey() const
    {
        return m_key;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & TextEntry::getValue() const
    {
        return m_text;
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
    float TextEntry::getMaxHeight() const
    {
        return m_maxHeight;
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