#pragma once

#include "Interface/DocumentInterface.h"
#include "Interface/TextBidirectionalInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct TextScriptSegmentDesc
    {
        size_t offset;
        size_t length;
        uint32_t scriptTag;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<TextScriptSegmentDesc> VectorTextScriptSegments;
    //////////////////////////////////////////////////////////////////////////
    class TextAlgorithmInterface
        : public Mixin
    {
    public:
        virtual TextBidirectionalInterfacePtr createBidirectional( const U32String & _text, const DocumentInterfacePtr & _doc ) const = 0;
        virtual bool getScriptSegments( const uint32_t * _text, size_t _size, VectorTextScriptSegments * const _segments ) const = 0;
        virtual Char32 getMirroredCodepoint( Char32 _code ) const = 0;
        virtual bool isCombiningMark( Char32 _code ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TextAlgorithmInterface> TextAlgorithmInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
