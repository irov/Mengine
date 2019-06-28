#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "BitmapGlyph.h"

namespace Mengine
{
    class BitmapGlyphServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "BitmapGlyphService" )

    public:
        virtual BitmapGlyphPtr getGlyph( const FileGroupInterfacePtr & _fileGroup, const FilePath & _path ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define BITMAPGLYPH_SERVICE()\
    ((Mengine::BitmapGlyphServiceInterface *)SERVICE_GET(Mengine::BitmapGlyphServiceInterface))
//////////////////////////////////////////////////////////////////////////