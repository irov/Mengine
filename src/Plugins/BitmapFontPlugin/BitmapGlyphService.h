#pragma once

#include "BitmapFontInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"
#include "Kernel/Map.h"

namespace Mengine
{
    class BitmapGlyphService
        : public ServiceBase<BitmapGlyphServiceInterface>
    {
    public:
        BitmapGlyphService();
        ~BitmapGlyphService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        BitmapGlyphPtr getGlyph( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath ) override;

    protected:
        FactoryPtr m_factoryTextGlyph;

        typedef Map<FilePath, BitmapGlyphPtr> MapBitmapGlyph;
        MapBitmapGlyph m_glyphs;
    };
}