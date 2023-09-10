#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/FileGroupInterface.h"

#include "BitmapFontGlyphDescription.h"

#ifndef MENGINE_BITMAP_METRICS_SYMBOL
#define MENGINE_BITMAP_METRICS_SYMBOL 'A'
#endif

namespace Mengine
{
    class BitmapFontServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "BitmapFontService" )

    public:
        virtual BitmapFontGlyphDescriptionPtr createGlyphDescription( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentInterfacePtr & _doc ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define BITMAPFONT_SERVICE()\
    ((Mengine::BitmapFontServiceInterface *)SERVICE_GET(Mengine::BitmapFontServiceInterface))
//////////////////////////////////////////////////////////////////////////