#pragma once

#include "Interface/TextFontInterface.h"

#include "Config/Vector.h"
#include "Config/VectorString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct TextLineChunk
    {
        TextLineChunk()
            :fontId( 555 )
        {
        };

        U32String value;
        uint32_t fontId;
    };
    //////////////////////////////////////////////////////////////////////////
    struct CacheFont
    {
        TextFontInterfacePtr font;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<TextLineChunk> VectorTextLineChunks;
    typedef Vector<VectorTextLineChunks> VectorTextLineChunks2;
    typedef Vector<CacheFont> VectorCacheFonts;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        bool test( VectorTextLineChunks & _out, const U32String & _in, VectorCacheFonts & _cacheFonts, uint32_t _font );
        void substr( VectorTextLineChunks & _out, const VectorTextLineChunks & _str, VectorTextLineChunks::size_type _offset, VectorTextLineChunks::size_type _size );
        void split( VectorTextLineChunks2 & _lines, const VectorTextLineChunks & _chunks, const VectorU32String & _delims );
    }
};
