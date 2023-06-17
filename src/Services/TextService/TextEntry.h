#pragma once

#include "Interface/TextEntryInterface.h"

#include "Kernel/Tags.h"
#include "Kernel/ConstString.h"
#include "Kernel/Color.h"

namespace Mengine
{
    class TextEntry
        : public TextEntryInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( TextEntry );

    public:
        TextEntry();
        ~TextEntry() override;

    public:
        bool initialize( const ConstString & _key
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
            , uint32_t _params );

    public:
        const ConstString & getKey() const override;
        const Char * getValue( size_t * const _size ) const override;

    public:
        const Tags & getTags() const override;

    public:
        const FontInterfacePtr & getFont() const override;
        const Color & getColorFont() const override;
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

        Tags m_tags;

        FontInterfacePtr m_font;

        Color m_colorFont;

        float m_lineOffset;
        float m_charOffset;
        float m_maxLength;

        ETextHorizontAlign m_horizontAlign;
        ETextVerticalAlign m_verticalAlign;

        float m_charScale;

        uint32_t m_params;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextEntry, TextEntryInterface> TextEntryPtr;
    //////////////////////////////////////////////////////////////////////////
}