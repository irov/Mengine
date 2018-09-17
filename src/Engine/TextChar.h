#pragma once

#include "Interface/TextInterface.h"

#include "Config/Vector.h"
#include "Config/VectorString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct TextChunk
    {
        TextChunk()
            :fontId( 555 ) {};

        U32String value;
        uint32_t fontId;
    };
    //////////////////////////////////////////////////////////////////////////
    struct CacheFont
    {
        TextFontInterfacePtr font;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Vector<TextChunk> VectorTextChunks;
    typedef Vector<VectorTextChunks> VectorTextLines;
    typedef Vector<CacheFont> VectorCacheFonts;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        bool test( VectorTextChunks & _out, const U32String & _in, VectorCacheFonts & _cacheFonts, uint32_t _font );
        void substr( VectorTextChunks & _out, const VectorTextChunks & _str, VectorTextChunks::size_type _offset, VectorTextChunks::size_type _size );
        void split( VectorTextLines & _lines, const VectorTextChunks & _chunks, const VectorU32String & _delims );
    }
};
