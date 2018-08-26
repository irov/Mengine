#pragma once

#include "Interface/TextInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/ColourValue.h"

#include "stdex/intrusive_tree.h"

namespace Mengine
{
    class TextEntry
        : public TextEntryInterface
    {
    public:
        TextEntry();

    public:
        void initialize( const ConstString & _key
            , const String & _text
            , const ConstString & _font
            , const ColourValue & _colorFont
            , float _lineOffset
            , float _charOffset
            , float _maxLength
            , ETextHorizontAlign _horizontAlign
            , ETextVerticalAlign _verticalAlign
            , float _charScale
            , uint32_t _params );

    public:
        const ConstString & getKey() const override;
        const String & getValue() const override;

    public:
        const ConstString & getFontName() const override;
        const ColourValue & getColorFont() const override;
        float getLineOffset() const override;
        float getCharOffset() const override;
        float getMaxLength() const override;
        ETextHorizontAlign getHorizontAlign() const override;
        ETextVerticalAlign getVerticalAlign() const override;
        float getCharScale() const override;

    public:
        uint32_t getFontParams() const override;

    protected:
        ConstString m_key;
        String m_text;

        ConstString m_fontName;

        ColourValue m_colorFont;

        float m_lineOffset;
        float m_charOffset;
        float m_maxLength;

        ETextHorizontAlign m_horizontAlign;
        ETextVerticalAlign m_verticalAlign;

        float m_charScale;

        uint32_t m_params;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextEntry> TextEntryPtr;
}