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
        , m_params( 0 )
    {
    }
	//////////////////////////////////////////////////////////////////////////
	TextEntry::~TextEntry()
	{

	}
	//////////////////////////////////////////////////////////////////////////
    void TextEntry::initialize( const ConstString & _key
        , const String & _text
        , const ConstString & _fontName
        , const ColourValue & _colorFont
        , float _lineOffset
        , float _charOffset
        , float _maxLength
        , ETextHorizontAlign _horizontAlign
        , ETextVerticalAlign _verticalAlign
        , float _charScale
        , uint32_t _params )
    {
        m_key = _key;
        m_text = _text;

        m_fontName = _fontName;

        m_colorFont = _colorFont;

        m_lineOffset = _lineOffset;
        m_charOffset = _charOffset;
        m_maxLength = _maxLength;

        m_horizontAlign = _horizontAlign;
        m_verticalAlign = _verticalAlign;

        m_charScale = _charScale;

        m_params = _params;
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
    const ConstString & TextEntry::getFontName() const
    {
        return m_fontName;
    }
    //////////////////////////////////////////////////////////////////////////
    const ColourValue & TextEntry::getColorFont() const
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
    uint32_t TextEntry::getFontParams() const
    {
        return m_params;
    }
}