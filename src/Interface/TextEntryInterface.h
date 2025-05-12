#pragma once

#include "Interface/FontInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/Tags.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/GlyphCode.h"
#include "Kernel/Color.h"
#include "Kernel/VectorString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ETextHorizontAlign
    {
        ETFHA_NONE = 0,
        ETFHA_LEFT = 1,
        ETFHA_CENTER = 2,
        ETFHA_RIGHT = 3,
        ETFHA_JUSTIFY = 4,        
    };
    //////////////////////////////////////////////////////////////////////////
    enum ETextVerticalAlign
    {
        ETFVA_NONE = 0,
        ETFVA_BOTTOM = 1,
        ETFVA_CENTER = 2,
        ETFVA_TOP = 3,
    };
    //////////////////////////////////////////////////////////////////////////
    class TextEntryInterface
        : public Mixin
    {
    public:
        virtual const ConstString & getKey() const = 0;
        virtual const Char * getValue( size_t * const _size ) const = 0;

    public:
        virtual const Tags & getTags() const = 0;

    public:
        virtual const FontInterfacePtr & getFont() const = 0;
        virtual const Color & getColorFont() const = 0;
        virtual float getLineOffset() const = 0;
        virtual float getCharOffset() const = 0;
        virtual float getMaxLength() const = 0;

        virtual ETextHorizontAlign getHorizontAlign() const = 0;
        virtual ETextVerticalAlign getVerticalAlign() const = 0;

        virtual float getCharScale() const = 0;

    public:
        virtual uint32_t getFontParams() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextEntryInterface> TextEntryInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
