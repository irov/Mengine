#pragma once

#include "Interface/TextInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct TextChunk
    {
        U32String value;
        uint32_t fontId;
    };
    //////////////////////////////////////////////////////////////////////////
    struct CacheFont
    {
        TextFontInterfacePtr font;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef stdex::vector<TextChunk> TVectorTextChunks;
    typedef stdex::vector<TVectorTextChunks> TVectorTextLines;
    typedef stdex::vector<CacheFont> TVectorCacheFonts;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        bool test( TVectorTextChunks & _out, const U32String & _in, TVectorCacheFonts & _cacheFonts, uint32_t _font );
        void substr( TVectorTextChunks & _out, const TVectorTextChunks & _str, TVectorTextChunks::size_type _offset, TVectorTextChunks::size_type _size );
        void split( TVectorTextLines & _lines, const TVectorTextChunks & _chunks, const TVectorU32String & _delims );
    }
};
