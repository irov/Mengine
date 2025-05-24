#pragma once

#include "Interface/FontInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/Tags.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/GlyphCode.h"
#include "Kernel/Color.h"
#include "Kernel/TextParams.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct TextEntryDesc
    {
        const Char * text;
        size_t size;
        Tags tags;
        FontInterfacePtr font;
        Color colorFont;
        float lineOffset;
        float charOffset;
        float maxLength;
        float maxHeight;
        ETextHorizontAlign horizontAlign;
        ETextVerticalAlign verticalAlign;
        float charScale;
        bool autoScale;
        bool justify;
        uint32_t params;
    };
    //////////////////////////////////////////////////////////////////////////
    class TextEntryInterface
        : public Mixin
    {
    public:
        virtual const ConstString & getKey() const = 0;
        virtual const String & getValue() const = 0;

    public:
        virtual const Tags & getTags() const = 0;

    public:
        virtual const FontInterfacePtr & getFont() const = 0;
        virtual const Color & getColorFont() const = 0;
        virtual float getLineOffset() const = 0;
        virtual float getCharOffset() const = 0;
        virtual float getMaxLength() const = 0;
        virtual float getMaxHeight() const = 0;

        virtual ETextHorizontAlign getHorizontAlign() const = 0;
        virtual ETextVerticalAlign getVerticalAlign() const = 0;

        virtual float getCharScale() const = 0;
        virtual bool getAutoScale() const = 0;
        virtual bool getJustify() const = 0;

    public:
        virtual uint32_t getFontParams() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextEntryInterface> TextEntryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
