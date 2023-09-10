#pragma once

#include "Interface/Interface.h"
#include "Interface/FontInterface.h"

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
        ETFHA_LEFT = 0,
        ETFHA_CENTER = 1,
        ETFHA_RIGHT = 2,
        ETFHA_NONE = 3
    };
    //////////////////////////////////////////////////////////////////////////
    enum ETextVerticalAlign
    {
        ETFVA_BOTTOM = 0,
        ETFVA_CENTER = 1,
        ETFVA_TOP = 2,
        ETFVA_NONE = 3,
    };
    //////////////////////////////////////////////////////////////////////////
    class TextEntryInterface
        : public Interface
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
