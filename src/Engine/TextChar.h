#pragma once

#include "Interface/FontInterface.h"

#include "Kernel/Vector.h"
#include "Kernel/VectorU32String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct TextLineChunk
    {
        U32String value;
        uint32_t fontId;
        ETextDirection direction = ETD_LEFT_TO_RIGHT;
        size_t logicalOffset = 0;
        size_t visualOrder = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    struct CacheFont
    {
        FontInterfacePtr font;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<TextLineChunk> VectorTextLineChunks;
    typedef Vector<VectorTextLineChunks> VectorTextLineChunks2;
    typedef Vector<CacheFont> VectorCacheFonts;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        bool test( VectorTextLineChunks * const _out, const U32String & _in, VectorCacheFonts * const _cacheFonts, uint32_t _font );
        void substr( VectorTextLineChunks * const _out, const VectorTextLineChunks & _str, VectorTextLineChunks::size_type _offset, VectorTextLineChunks::size_type _size );
        void split( VectorTextLineChunks2 * const _lines, const VectorTextLineChunks & _chunks, const VectorU32String & _delims );
    }
    //////////////////////////////////////////////////////////////////////////
};
