#pragma once

#include "Interface/DataInterface.h"
#include "Interface/MemoryInterface.h"

#include "ft2build.h"
#include "freetype/freetype.h"
#include "freetype/ftglyph.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TTFDataInterface
        : public DataInterface
    {
    public:
        virtual const MemoryInterfacePtr & getTTFMemory() const = 0;
        virtual FT_Face getFTFace() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<TTFDataInterface, DataInterface> TTFDataInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}