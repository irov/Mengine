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
        bool initialize( const ConstString & _key, const TextEntryDesc & _desc );

    public:
        const ConstString & getKey() const override;
        const String & getValue() const override;

    public:
        const Tags & getTags() const override;

    public:
        const FontInterfacePtr & getFont() const override;
        const Color & getColorFont() const override;
        float getLineOffset() const override;
        float getCharOffset() const override;
        float getMaxLength() const override;
        float getMaxHeight() const override;
        ETextHorizontAlign getHorizontAlign() const override;
        ETextVerticalAlign getVerticalAlign() const override;
        float getCharScale() const override;
        bool getAutoScale() const override;
        bool getJustify() const override;

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
        float m_maxHeight;

        ETextHorizontAlign m_horizontAlign;
        ETextVerticalAlign m_verticalAlign;

        float m_charScale;
        bool m_autoScale;
        bool m_justify;

        uint32_t m_params;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextEntry, TextEntryInterface> TextEntryPtr;
    //////////////////////////////////////////////////////////////////////////
}