#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/TextParams.h"
#include "Kernel/U32String.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct TextBidirectionalSegmentDesc
    {
        size_t offset;
        size_t length;
        ETextDirection direction;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<TextBidirectionalSegmentDesc> VectorTextBidirectionalSegments;
    //////////////////////////////////////////////////////////////////////////
    class TextBidirectionalInterface
        : public Mixin
    {
    public:
        virtual bool getVisualSegments( size_t _begin, size_t _end, VectorTextBidirectionalSegments * const _segments ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextBidirectionalInterface> TextBidirectionalInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
